#include "headers_and_defines.h"

struct mips_cpu_impl
{
    uint32_t pc;
    uint32_t pcNext;
    
    uint32_t regs[32];
    
    mips_mem_h mem;
    
    unsigned logLevel;
    FILE *logDst;
};

struct instruction {
	
	instruction(uint32_t inst_in) {
		whole = inst_in;
		opcode =  (inst_in>>26) & 0x3F;
		src1 = (inst_in>> 21 ) & 0x1F;
		src2 = (inst_in>> 16 ) & 0x1F;   
		dst = (inst_in>> 11 ) & 0x1F;    
		shift = (inst_in>> 6 ) & 0x1F ;
		function = (inst_in>> 0 ) & 0x3F;
		if (opcode == 0) {
			type = 1;
		} else if (opcode == 2 || opcode == 3) {
			type = 3;
		} else {
			type = 2;
		}
	}
	
	uint32_t whole;
	uint32_t opcode;
    uint32_t src1;
    uint32_t src2; 
    uint32_t dst;
    uint32_t shift;
    uint32_t function;
	int type; // R = 1, I = 2, J = 3 
};


mips_error mips_cpu_get_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t *value		//!< Where to write the value to
)
{
    try {
		*value = state->regs[index];
	} catch (mips_error e) {
		return e;
	}
    
    return mips_Success;
}

/*! Modifies one of the 32 general purpose MIPS registers. */
mips_error mips_cpu_set_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t value		//!< New value to write into register file
)
{
	try {
		state->regs[index] = value;
	} catch (mips_error e) {
		return e;
	}

    return mips_Success;
}

mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level, FILE *dest)
{
    state->logLevel = level;
    state->logDst = dest;
    return mips_Success;
}

mips_cpu_h mips_cpu_create(mips_mem_h mem)
{
    mips_cpu_impl *cpu=new mips_cpu_impl;
    
    cpu->pc=0;
	cpu->pcNext = 4; 
    for(int i=0;i<32;i++){
        cpu->regs[i]=0;
    }
    cpu->mem=mem;
    
    return cpu;
}

/*! \param pData Array of 4 bytes
    \retval The big-endian equivlent
*/
uint32_t to_big(const uint8_t *pData)
{
    return
        (((uint32_t)pData[0])<<24)
        |
        (((uint32_t)pData[1])<<16)
        |
        (((uint32_t)pData[2])<<8)
        |
        (((uint32_t)pData[3])<<0);
}

int subtype1(string name, instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 1){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", name.c_str(), inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	if (name == "addu") {
		res=va+vb;
		state->regs[inst.dst] = res;
	} else if (name == "and") {
		res=va&vb;
		state->regs[inst.dst] = res;
	} else if (name == "or") {
		res=va|vb;
		state->regs[inst.dst] = res;
	} else if (name == "xor") {
		res=va^vb;
		state->regs[inst.dst] = res;
	} else if (name == "sltu") {
		if (vb > va) {state->regs[inst.dst] = 0x01;}
		else {state->regs[inst.dst] = 0x00;}
	}

	return 4;
	
}

/*
int addu(instruction inst, mips_cpu_h state) {

}

int and_(instruction inst, mips_cpu_h state) {
    if(state->logLevel >= 1){
		fprintf(state->logDst, "and %u, %u, %u.\n", inst.dst, inst.src1, inst.src2);
	}
	uint32_t va=state->regs[inst.src1];
	uint32_t vb=state->regs[inst.src2];

	uint32_t res=va&vb;
            
	state->regs[inst.dst] = res;
	return 4;
}
int or_(instruction inst, mips_cpu_h state) {
    if(state->logLevel >= 1){
		fprintf(state->logDst, "and %u, %u, %u.\n", inst.dst, inst.src1, inst.src2);
	}
	uint32_t va=state->regs[inst.src1];
	uint32_t vb=state->regs[inst.src2];

	uint32_t res=va|vb;
            
	state->regs[inst.dst] = res;
	return 4;
}

int xor_(instruction inst, mips_cpu_h state) {
    if(state->logLevel >= 1){
		fprintf(state->logDst, "and %u, %u, %u.\n", inst.dst, inst.src1, inst.src2);
	}
	uint32_t va=state->regs[inst.src1];
	uint32_t vb=state->regs[inst.src2];

	uint32_t res=va^vb;
            
	state->regs[inst.dst] = res;
	return 4;
}
*/


mips_error mips_cpu_step(
	mips_cpu_h state	//! Valid (non-empty) handle to a CPU
)
{
	
    uint8_t buffer[4];
    
    mips_error err=mips_mem_read(
        state->mem,		//!< Handle to target memory
        state->pc,	//!< Byte address to start transaction at
        4,	//!< Number of bytes to transfer
        buffer	//!< Receives the target bytes
    );
    
    if(err!=0){
        return err;
    }
    
    instruction inst(to_big(buffer));
    int offset = 0;

    if(inst.type == 1){
        if(state->logLevel >= 2){
            fprintf(state->logDst, "R-Type : dst=%u, src1=%u, src2=%u, shift=%u, function=%u.\n  instr=%08x\n",
                inst.dst, inst.src1, inst.src2, inst.shift, inst.function, inst.whole
            );
        }
        
        if(inst.function ==  0x21){
			offset = subtype1("addu",inst,state);
        } else if(inst.function ==  0x24) {
			offset = subtype1("and",inst,state);
		} else if(inst.function ==  0x25) {
			offset = subtype1("or",inst,state);
		} else if(inst.function ==  0x26) {
			offset = subtype1("xor",inst,state);
		} else if(inst.function ==  0x28){
            offset = subtype1("sltu",inst,state);
        }
    } else {
        return mips_ErrorNotImplemented;
    }
	
	// TODO : What about updating the program counter
	state->regs[0] = 0;
	//state->pc = state->pcNext;
	//state->pcNext += offset;
	return mips_Success;
}
