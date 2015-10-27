#include <stdlib.h>
#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <map>
#include "mips.h"
using namespace std;

bool sig_to_unsig(uint16_t sig, uint16_t* unsig);

//
struct mips_cpu_impl {
    uint32_t pc;
    uint32_t pcNext;
    uint32_t regs[32];
    mips_mem_h mem;
    unsigned logLevel;
    FILE *logDst;
};

//
struct instruction {
	instruction(uint32_t inst_in) {
		whole = inst_in;
		opcode =  (inst_in>>26) & 0x3F;
		src1 = (inst_in>> 21 ) & 0x1F;
		src2 = (inst_in>> 16 ) & 0x1F;   
		dst = (inst_in>> 11 ) & 0x1F;    
		shift = (inst_in>> 6 ) & 0x1F ;
		function = (inst_in>> 0 ) & 0x3F;
		ioffset = (inst_in>>0) & 0xFFFF;
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
	uint32_t ioffset;
	int type; // R = 1, I = 2, J = 3 
};

bool sig_to_unsig(uint16_t sig, uint16_t* unsig) {
    
	bool negative = false;
	if ((sig & 0x8000) != 0) {
		negative = true;
		*unsig = ~sig + 1;
	} else {
		*unsig = sig;
	}
	return negative;

}

void mips_cpu_free(mips_cpu_h state)
{
	if(state != NULL)
	{
		if(state->logDst != NULL)
			fclose(state->logDst);
		free(state);
	}
}

mips_error mips_cpu_set_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t pc			//!< Address of the next instruction to exectute.
) {
	
	try {
		state->pc = pc;
	} catch (mips_error e) {
		return e;
	}
    
   	return mips_Success;
}
mips_error mips_cpu_get_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t *pc		//!< Where to write the byte address too
) {
	
	*pc = state->pc;
	try {
		//*pc = new uint32_t;
		*pc = state->pc;
	} catch (mips_error e) {
		
		return e;
	}
   	return mips_Success;
}

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


uint32_t _addu(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va+vb;
	state->regs[inst.dst] = res;

	return 4;
}

uint32_t _and(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va&vb;
	state->regs[inst.dst] = res;

	return 4;
}

uint32_t _or(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va|vb;
	state->regs[inst.dst] = res;

	return 4;
}

uint32_t _xor(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va^vb;
	state->regs[inst.dst] = res;

	return 4;
}

uint32_t _sltu(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.dst, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	if (vb > va) {state->regs[inst.dst] = 0x01;}
	else {state->regs[inst.dst] = 0x00;}
	return 4;
}

uint32_t beq(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src1, inst.src2, inst.ioffset);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t retval = 4;
	if (va == vb) {
		retval = inst.ioffset << 2;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "pc offset = %x\n", retval);
	}
	return retval;
}

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
    
    if(err!=0) {
        return err;
    }
    
    instruction inst(to_big(buffer));
    int offset = 0;
	
    if(state->logLevel >= 3) {
		fprintf(state->logDst, "Current pc = %x\n", state->pc);
		fprintf(state->logDst, "type = %i\n", inst.opcode);
    }
	
    if(inst.type == 1) {
        if(state->logLevel >= 3) {
            fprintf(state->logDst, "R-Type : dst=%u, src1=%u, src2=%u, shift=%u, function=%u.\n  instr=%08x\n",
                inst.dst, inst.src1, inst.src2, inst.shift, inst.function, inst.whole
            );
        }
        if(inst.function ==  0x21){
			offset = _addu(inst,state);
        } else if(inst.function ==  0x24) {
			offset = _and(inst,state);
		} else if(inst.function ==  0x25) {
			offset = _or(inst,state);
		} else if(inst.function ==  0x26) {
			offset = _xor(inst,state);
		} else if(inst.function ==  0x28) {
			offset = _sltu(inst,state);
        }
    } else if(inst.type == 2) {
       	if(state->logLevel >= 3) {
            fprintf(state->logDst, "I-Type: src1=%u, src2=%u, ioffset=%u\n", inst.src1, inst.src2, inst.ioffset );
        }
		if(inst.opcode ==  4){
			offset = beq(inst, state);
		}
		
    } else {
	return mips_ErrorNotImplemented;
    }
	state->regs[0] = 0;
	state->pc = state->pcNext;
	uint16_t unsig_offset;
	bool negative = sig_to_unsig(offset,&unsig_offset); 
	if (negative) {
		state->pcNext -= unsig_offset;
	} else {
		state->pcNext += unsig_offset;
	}
	
	if(state->logLevel >= 3) {
            fprintf(state->logDst, "pcNext = %x\n", state-> pcNext);
    }
	return mips_Success;
}
