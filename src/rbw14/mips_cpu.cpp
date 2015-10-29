#include <stdlib.h>
#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <map>
#include <math.h>
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
		opcode =  inst_in>>26 ;
		src1 = (inst_in>> 21 ) & 0x1F;
		src2 = (inst_in>> 16 ) & 0x1F;   
		src3 = (inst_in>> 11 ) & 0x1F;    
		shift = (inst_in>> 6 ) & 0x1F ;
		function = (inst_in>> 0 ) & 0x3F;
		idata = (inst_in>>0) & 0xFFFF;
		
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
   	uint32_t src3;
    uint32_t shift;
   	uint32_t function;
	uint32_t idata;
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

mips_error mips_cpu_reset(mips_cpu_h state) {
	int i;
	state->pc = 0;
	state->pcNext = 4;
	for (i = 0; i < 32; i++) {
		state->regs[i] = 0;
	}
	return mips_Success;
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
	if(state==0)
		return mips_ErrorInvalidHandle;
	if(index>=32)
		return mips_ErrorInvalidArgument;
	if(index == 0)
		 state->regs[index]=0;
	if(index != 0)
		state->regs[index]=value;
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
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va+vb;
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _add(instruction inst, mips_cpu_h state, mips_error& e) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va+vb;
	uint32_t va_sign = va & 0x80000000;
	uint32_t vb_sign = vb & 0x80000000;
	uint32_t res_sign = res & 0x80000000;
	if ((va_sign == 0 && vb_sign == 0 && res_sign != 0)||(va_sign != 0 && vb_sign != 0 && res_sign == 0)) {
		e = mips_ExceptionArithmeticOverflow;
		return 4;
	}
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _sub(instruction inst, mips_cpu_h state, mips_error& e) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res = va + (~vb + 1);
	uint32_t va_sign = va & 0x80000000;
	uint32_t vb_sign = vb & 0x80000000;
	uint32_t res_sign = res & 0x80000000;
	if ((va_sign == 0 && vb_sign != 0 && res_sign != 0)||(va_sign != 0 && vb_sign == 0 && res_sign == 0)) {
		e = mips_ExceptionArithmeticOverflow;
		return 4;
	}
	
	state->regs[inst.src3] = res;

	return 4;
} 

uint32_t _slt(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res = va + (~vb + 1);
	uint32_t va_sign = va & 0x80000000;
	uint32_t vb_sign = vb & 0x80000000;
	uint32_t res_sign = res & 0x80000000;
	if (va_sign == 0 && vb_sign != 0) {
		state->regs[inst.src3] = 0;
	} else if (va_sign != 0 && vb_sign == 0) {
		state->regs[inst.src3] = 1;
	} else if (res_sign != 0) {
		state->regs[inst.src3] = 1;
	} else if (res_sign == 0) {
		state->regs[inst.src3] = 0;
	}

	return 4;
}


uint32_t _and(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va&vb;
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _andi(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.idata);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res;
	res=va&vb;
	state->regs[inst.src2] = res;
	return 4;
}

uint32_t _ori(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.idata);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res;
	res=va|vb;
	state->regs[inst.src2] = res;
	return 4;
}

uint32_t _xori(instruction inst, mips_cpu_h state) {	
	cout << bitset<32>(inst.whole) << endl;
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.idata);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res;
	res=va^vb;
	state->regs[inst.src2] = res;
	cout << hex << inst.src2 << endl;
	return 4;
}

uint32_t _addiu(instruction inst, mips_cpu_h state) {
	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res;
	res=va+vb;
	state->regs[inst.src2] = res;
	return 4;
}



uint32_t _addi(instruction inst, mips_cpu_h state, mips_error& e) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.idata, inst.src1, inst.src2);
	}
	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res = va + vb;
	uint32_t va_sign = va & 0x80000000;
	uint32_t vb_sign = vb & 0x80000000;
	uint32_t res_sign = res & 0x80000000;
	if ((va_sign == 0 && vb_sign == 0 && res_sign != 0)||(va_sign != 0 && vb_sign != 0 && res_sign == 0)) {
		e = mips_ExceptionArithmeticOverflow;
		return 4;
	}
	state->regs[inst.src2] = res;
	return 4;
}

uint32_t _lui(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.idata, inst.src1, inst.src2);
	}

	uint32_t vb = inst.idata;
	state->regs[inst.src2] = vb << 16;	
	return 4;
}

uint32_t _lbu(instruction inst, mips_cpu_h state, mips_error& e) {

	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.idata, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t address = va + vb;

	
	uint8_t buffer[4];
	e = mips_mem_read(state->mem, address, 4, buffer );
	
	if (e != 0) {
		return 4;
	}
	
	uint32_t res = (uint32_t(buffer[0]));
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : va = %i , vb = %i ,address = %i, res = %x", __func__, va, vb, address, res);
	}

	e = mips_cpu_set_register(state,inst.src2,res);
		
	return 4;
}

uint32_t _lw(instruction inst, mips_cpu_h state, mips_error& e) {

	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.idata, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t address = va + vb;

	
	uint8_t buffer[4];
	e = mips_mem_read(state->mem, address, 4, buffer );
	
	if (e != 0) {
		return 4;
	}
	uint32_t res = (uint32_t(buffer[0]) << 24) | (uint32_t(buffer[1]) << 16) | (uint32_t(buffer[2]) << 8) | (uint32_t(buffer[3]));
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : va = %i , vb = %i ,address = %i, res = %x", __func__, va, vb, address, res);
	}
	e = mips_cpu_set_register(state,inst.src2,res);
		
	return 4;
}

uint32_t _sw(instruction inst, mips_cpu_h state, mips_error& e) {
	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.idata, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t address = va + vb;
	uint32_t stored = state->regs[inst.src2];
	
	
	uint8_t buffer[4];
	buffer[0] = stored >> 24 & 0xFF;
	buffer[1] = (stored >> 16) & 0xFF;
	buffer[2] = (stored >> 8) & 0xFF;
	buffer[3] = (stored) & 0xFF;
	
	e = mips_mem_write(state->mem, address, 4, buffer);

		
	return 4;
}

uint32_t _sltiu(instruction inst, mips_cpu_h state) {
	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	if (vb > va) {state->regs[inst.src2] = 0x01;}
	else {state->regs[inst.src2] = 0x00;}
	return 4;
}

uint32_t _slti(instruction inst, mips_cpu_h state) {
	if ((inst.idata & 0x8000) != 0) {
		inst.idata = inst.idata | 0xFFFF0000;
	}
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = inst.idata;
	uint32_t res;
	res = va + (~vb + 1);
	uint32_t va_sign = va & 0x80000000;
	uint32_t vb_sign = vb & 0x80000000;
	uint32_t res_sign = res & 0x80000000;
	if (va_sign == 0 && vb_sign != 0) {
		state->regs[inst.src2] = 0;
	} else if (va_sign != 0 && vb_sign == 0) {
		state->regs[inst.src2] = 1;
	} else if (res_sign != 0) {
		state->regs[inst.src2] = 1;
	} else if (res_sign == 0) {
		state->regs[inst.src2] = 0;
	}

	return 4;
}

uint32_t _or(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va|vb;
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _xor(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va^vb;
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _sltu(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	if (vb > va) {state->regs[inst.src3] = 0x01;}
	else {state->regs[inst.src3] = 0x00;}
	return 4;
}

uint32_t _subu(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=va-vb;
	state->regs[inst.src3] = res;

	return 4;
}

uint32_t _sra(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	uint32_t sign_bit = vb & 0x80000000;
	if (sign_bit == 0) {
		res =(vb>>inst.shift);
	} else {
		
		uint32_t ones = (uint32_t(pow(2, inst.shift)) - 1) << (32-inst.shift);
		res = (vb>>inst.shift) | ones;
		if(state->logLevel >= 3){
			fprintf(state->logDst, "%s: shift = %x, ones = %x, in = %x, out = %x\n", __func__, inst.shift, ones, vb, res);
		}
	}
	
	state->regs[inst.src3] = res;
	return 4;
}

uint32_t _srav(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	uint32_t sign_bit = vb & 0x80000000;
	uint32_t shift = va & 0x1F;
	if (sign_bit == 0) {
		res =(vb>>shift);
	} else {
		
		uint32_t ones = (uint32_t(pow(2, shift)) - 1) << (32-shift);
		res = (vb>>shift) | ones;
	}
	
	state->regs[inst.src3] = res;
	return 4;
}

uint32_t _srl(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res =(vb>>inst.shift);
	
	state->regs[inst.src3] = res;
	return 4;
}

uint32_t _srlv(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=vb>>(va & 0x1F);
	state->regs[inst.src3] = res;
	return 4;
}

uint32_t _sll(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=vb<<inst.shift;
	state->regs[inst.src3] = res;
	return 4;
}


uint32_t _sllv(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src3, inst.src1, inst.src2);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t res;
	res=vb<<(va & 0x1F);
	state->regs[inst.src3] = res;
	return 4;
}

uint32_t beq(instruction inst, mips_cpu_h state) {
	if(state->logLevel >= 2){
		fprintf(state->logDst, "%s : %u, %u, %u.\n", __func__, inst.src1, inst.src2, inst.idata);
	}
	uint32_t va = state->regs[inst.src1];
	uint32_t vb = state->regs[inst.src2];
	uint32_t retval = 4;
	if (va == vb) {
		retval = inst.idata << 2;
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
    instruction inst = (uint32_t(buffer[0]) << 24 | uint32_t(buffer[1]) << 16 | uint32_t(buffer[2]) << 8 | uint32_t(buffer[3]) << 0);
	
    int offset;
	
    if(state->logLevel >= 3) {
		fprintf(state->logDst, "Current pc = %x\n", state->pc);
		fprintf(state->logDst, "type = %i\n", inst.opcode);
    }
    if(inst.type == 1) {
		if (inst.opcode != 0x00 && state -> logLevel >= 1) {
			fprintf(state->logDst, "R TYPE BAD DECODING");
		}
        if(state->logLevel >= 3) {
            fprintf(state->logDst, "R-Type : src3=%u, src1=%u, src2=%u, shift=%u, function=%u.\n  instr=%08x\n",
                inst.src3, inst.src1, inst.src2, inst.shift, inst.function, inst.whole
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
		} else if(inst.function ==  0x2B) {
			offset = _sltu(inst,state);
        } else if(inst.function ==  0x23) {
			offset = _subu(inst,state);
		} else if(inst.function ==  0x00) {
			offset = _sll(inst,state);
		} else if(inst.function ==  0x02) {
			offset = _srl(inst,state);
		} else if(inst.function ==  0x04) {
			offset = _sllv(inst,state);
		} else if(inst.function ==  0x06) {
			offset = _srlv(inst,state);
		} else if(inst.function ==  0x03) {
			offset = _sra(inst,state);
		} else if(inst.function ==  0x07) {
			offset = _srav(inst,state);
		} else if(inst.function ==  0x20) {
			offset = _add(inst,state,err);
		} else if(inst.function ==  0x22) {
			offset = _sub(inst,state,err);
		} else if(inst.function ==  0x2A) {
			offset = _slt(inst,state);
		} 
		
	} if (inst.type == 2) {
		if (inst.opcode == 0x0C) {
			offset = _andi(inst,state);
		} else if (inst.opcode == 0x0D) {
			offset = _ori(inst,state);
		} else if (inst.opcode == 0x0E) {
			offset = _xori(inst,state);
		} else if (inst.opcode == 0x09) {
			offset = _addiu(inst,state);
		} else if (inst.opcode == 0x08) {
			offset = _addi(inst,state,err);
		} else if (inst.opcode == 0x0B) {
			offset = _sltiu(inst,state);
		} else if (inst.opcode == 0x0A) {
			offset = _slti(inst,state);
		} else if (inst.opcode == 0x23) {
			offset = _lw(inst,state,err);
		} else if (inst.opcode == 0x0F) {
			offset = _lui(inst,state);
		} else if (inst.opcode == 0x2B) {
			offset = _sw(inst,state,err);
		}else if (inst.opcode == 0x24) {
			offset = _lbu(inst,state,err);
		} if (inst.opcode == 0x04) {
			//cout << "BEQ" << endl;
		} 
		
	}
	if (err != mips_Success) {
		return err;
	}
	state->regs[0] = 0;
	
	
	state->pc = state->pcNext;
	state->pcNext += offset;

	
	if(state->logLevel >= 3) {
            fprintf(state->logDst, "pcNext = %x\n", state-> pcNext);
    }
	return mips_Success;
}
