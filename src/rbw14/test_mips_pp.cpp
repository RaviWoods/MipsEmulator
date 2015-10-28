#include "mips_test.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <iostream>

void word_to_byte(uint32_t word, uint8_t *bytes);
uint32_t byte_to_word(uint8_t *bytes);

uint32_t r_type_create(std::string instruction, uint32_t src1, uint32_t src2,uint32_t dest, uint32_t shift);
void r_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t src1, uint32_t value1, uint32_t src2, uint32_t value2, uint32_t dest, uint32_t shift);

void i_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t src1, uint32_t value1, uint32_t dest, int16_t imm, uint32_t value2);
uint32_t i_type_create(std::string instruction, uint32_t src1, uint32_t dest, int16_t imm);

uint32_t j_type_create(std::string instruction, uint32_t addr);
void j_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t addr);

void test_set_zeroins(mips_cpu_h cpu, mips_mem_h mem);
uint32_t test_get_jump_pc(mips_cpu_h cpu, mips_mem_h mem);

int main()
{
	mips_mem_h mem=mips_mem_create_ram(1<<20,4);

	mips_cpu_h cpu=mips_cpu_create(mem);

	mips_test_begin_suite();
	
	mips_cpu_set_pc(cpu,32);
	
	mips_cpu_set_register(cpu,16,8546);
	i_ins(cpu,mem,"beq",1,5,2,2,5);
	i_ins(cpu,mem,"beq",1,5,2,-2,5);	
	i_ins(cpu,mem,"beq",1,5,2,-2,4);
	/*
	i_ins(cpu,mem,"sw",17,80,16,-4,0);
	i_ins(cpu,mem,"sw",17,36,16,0,0);
	i_ins(cpu,mem,"sw",17,44,16,4,0);
	
	i_ins(cpu,mem,"lui",0,0,5,0x8FFF,0);
	
	
	r_ins(cpu,mem,"addu",1,0x7fffffff,2,1,20,0); // addu $20, $1, $2 - overflow		
	r_ins(cpu,mem,"add",1,0x7fffffff,2,1,21,0); // add $21, $1, $2 - overflow
	
	r_ins(cpu,mem,"addu",3,0x80000001,4,0x80000001,22,0); // addu $22, $3, $4 - overflow	
	r_ins(cpu,mem,"add",3,0x80000001,4,0x80000001,23,0); // add $23, $3, $4 - overflow	
	
	r_ins(cpu,mem,"addu",5,0x12345678,6,0x11111111,24,0); // add $24, $5, $6 - no overflow	
	r_ins(cpu,mem,"add",5,0x12345678,6,0x11111111,25,0); // add $25, $5, $6 - no overflow
	
	r_ins(cpu,mem,"sub",7,0x80000000,8,1,26,0); // sub $26, $7, $8 - overflow
	r_ins(cpu,mem,"subu",7,0x80000000,8,1,26,0); // subu $26, $7, $8 - overflow
	r_ins(cpu,mem,"sub",7,15,8,1,26,0); // sub $26, $7, $8 - no overflow
	r_ins(cpu,mem,"subu",7,15,8,1,26,0); // subu $26, $7, $8 - no overflow
	
	// xor $1, $2, $3

	r_ins(cpu,mem,"xor",2,0xFF,3,0xFF005678,1,0);

	// or $7, $8, $9

	r_ins(cpu,mem,"or",8,0xFF,9,0xFF005678,7,0);

	// and $10, $11, $12

	r_ins(cpu,mem,"and",11,0xFF,12,0xFF005678,10,0);

	// subu $13, $14, $15

	r_ins(cpu,mem,"subu",14,0xFFFFFFFF,15,0xFF005678,13,0);
	r_ins(cpu,mem,"subu",14,5679,15,6000,13,0);
		// lw $16, $17, 40

	uint8_t data[4];
	data[0] = 8;

	mips_mem_write(mem, 36, 4, data);
	mips_mem_write(mem, 40, 4, data);

	i_ins(cpu,mem,"lw",17,40,16,-4,0);
	i_ins(cpu,mem,"lw",17,40,16,0,0);
	
	//i_ins(cpu,mem,"lw",17,40,16,4);
	// lw $16, $17, 40	
	
	

	i_ins(cpu,mem,"xori",0,0x5678,2,0x8765,0);
	i_ins(cpu,mem,"xori",1,0x5678,2,0x8765,0);

	i_ins(cpu,mem,"ori",0,0x5678,2,0x8765,0);
	i_ins(cpu,mem,"ori",1,0x5678,2,0x8765,0);
	
	mips_cpu_set_pc(cpu,0x4);

	i_ins(cpu,mem,"andi",0,0x5678,2,0x8765,0);
	i_ins(cpu,mem,"andi",1,0x5678,2,0x8765,0);

	i_ins(cpu,mem,"addiu",5,40,4,20,0);
	i_ins(cpu,mem,"addiu",5,40,4,-20,0);
	i_ins(cpu,mem,"addiu",5,40,4,-60,0);
	
	mips_cpu_set_pc(cpu,0x4);
	
	i_ins(cpu,mem,"addi",27,1235985687,4,60797,0);
	i_ins(cpu,mem,"addi",5,1,4,1,0);
	
	mips_cpu_set_pc(cpu,0x8);

	r_ins(cpu,mem,"sll",0,0,7,0xFFFFFFFF,15,8);
	r_ins(cpu,mem,"sll",0,0,7,0xFFFFFFFF,15,0);
	r_ins(cpu,mem,"sll",0,0,7,0xFFFFFFFF,15,31);

	r_ins(cpu,mem,"sllv",6,8,7,0xFFFFFFFF,15,0);
	r_ins(cpu,mem,"sllv",6,32,7,0xFFFFFFFF,15,0);

	r_ins(cpu,mem,"sltu",6,1,7,2,15,0);
	r_ins(cpu,mem,"sltu",6,2,7,1,15,0);
	
	j_ins(cpu,mem,"j",4);
	j_ins(cpu,mem,"jal",1);
	
	i_ins(cpu,mem,"beq",1,5,2,2,5);
	i_ins(cpu,mem,"beq",1,5,2,-2,5);	
	i_ins(cpu,mem,"beq",1,5,2,-2,4);
	
	i_ins(cpu,mem,"bne",1,5,2,2,1);
	i_ins(cpu,mem,"bne",1,5,2,-2,5);
	
	i_ins(cpu,mem,"bgtz",1,5,2,2,0);
	i_ins(cpu,mem,"bgtz",1,0,2,2,0);
	i_ins(cpu,mem,"bgtz",1,-5,2,2,0);
	i_ins(cpu,mem,"bgtz",1,5,2,-2,0);
	
	i_ins(cpu,mem,"blez",1,0,2,-2,0);
	i_ins(cpu,mem,"blez",1,-1,2,-2,0);
	i_ins(cpu,mem,"blez",1,1,2,-2,0);
	
	i_ins(cpu,mem,"bgez",1,5,2,2,0);
	i_ins(cpu,mem,"bgez",1,0,2,2,0);
	i_ins(cpu,mem,"bgez",1,-5,2,2,0);
	i_ins(cpu,mem,"bgez",1,5,2,-2,0);
	
	i_ins(cpu,mem,"bgezal",1,5,2,2,0);
	i_ins(cpu,mem,"bgezal",1,0,2,2,0);
	i_ins(cpu,mem,"bgezal",1,-5,2,2,0);
	i_ins(cpu,mem,"bgezal",1,5,2,-2,0);
	
	i_ins(cpu,mem,"bltz",1,5,2,2,0);
	i_ins(cpu,mem,"bltz",1,0,2,2,0);
	i_ins(cpu,mem,"bltz",1,-5,2,2,0);
	i_ins(cpu,mem,"bltz",1,5,2,-2,0);
	
	i_ins(cpu,mem,"bltzal",1,5,2,2,0);
	i_ins(cpu,mem,"bltzal",1,0,2,2,0);
	i_ins(cpu,mem,"bltzal",1,-5,2,2,0);
	i_ins(cpu,mem,"bltzal",1,5,2,-2,0);
	
	r_ins(cpu,mem,"jr",6,80,0,0,15,0);
	
	r_ins(cpu,mem,"jalr",6,40,0,0,0,0);
	
	
	r_ins(cpu,mem,"jalr",6,36,0,0,15,0);
	
	//uint32_t gotr,gotr2;
	//mips_cpu_get_register(cpu,15,&gotr);
	//std::cout << "register 15 state " << gotr << std::endl;
	
	/*uint32_t result;
	mips_cpu_get_register(cpu,15,&result);
	printf("%X\n", result);
    */
	
	mips_test_end_suite();

	mips_cpu_free(cpu);
	mips_mem_free(mem);

	return 0;
}

uint32_t r_type_create(std::string instruction, uint32_t src1, uint32_t src2, uint32_t dest, uint32_t shift){

	uint32_t opcode = 0;
	uint32_t fn = 0;

	if(instruction == "sll")
		fn = 0x0;
	else if(instruction == "sllv")
		fn = 0x4;
	else if(instruction == "jr")
		fn = 0x8;
	else if(instruction == "jalr")
		fn = 0x9;
	else if(instruction == "add")
		fn = 0x20;
	else if(instruction == "addu")
		fn = 0x21;
	else if(instruction == "sub")
		fn = 0x22;
	else if(instruction == "subu")
		fn = 0x23;
	else if(instruction == "and")
		fn = 0x24;
	else if(instruction == "or")
		fn = 0x25;
	else if(instruction == "xor")
		fn = 0x26;
    else if(instruction == "sltu")
        fn = 0x2B;

	return (opcode << 26 | src1 << 21 | src2 << 16 | dest << 11 | shift << 6 | fn);

}

void word_to_byte(uint32_t word, uint8_t *bytes){
	for(unsigned int i = 0;i < 4;i++){
		bytes[i] = (word >> (8*(3-i)));
	}
}

uint32_t byte_to_word(uint8_t *bytes){
	uint32_t bytes_cast[4];
	for (unsigned i=0;i<4;i++){
		bytes_cast[i] = bytes[i];
	}
	return (bytes_cast[0] << 24 | bytes_cast[1] << 16 | bytes_cast[2] << 8 | bytes_cast[3]);
}

void r_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t src1, uint32_t value1, uint32_t src2, uint32_t value2, uint32_t dest, uint32_t shift){

	int testId=mips_test_begin_test(instruction.c_str());
	int passed=0;
	
	mips_error err = mips_cpu_set_register(cpu, src1, value1);
	if(err==0)
		err = mips_cpu_set_register(cpu, src2, value2);

	uint8_t bytes[4];

	if(err==0)
	word_to_byte(r_type_create(instruction,src1,src2,dest,shift),bytes);

	
	uint32_t pc;
	err = mips_cpu_get_pc(cpu,&pc);

	if(err==0)
	err = mips_mem_write(mem,pc,4,bytes);
	
	

	uint32_t dest_init, got;
	
	if(src1 == 0)
		value1 = 0;
	if(src2 == 0)
		value2 = 0;

	if(err==0)
		err = mips_cpu_get_register(cpu, dest, &dest_init);	

	if(err==0)
		err=mips_cpu_step(cpu);
		
	mips_cpu_get_register(cpu, dest, &got);	
	
	if(instruction == "sll")
		passed = (err == mips_Success) && (got == (value2 << shift)) | ((got == 0) && (dest == 0));
		
	else if(instruction == "sllv"){
        passed = (err == mips_Success) && (got == (value2 << (value1 & 0x1F))) | ((got == 0) && (dest == 0));
    }
    
    else if(instruction == "jr"){
    	uint32_t new_pc = test_get_jump_pc(cpu,mem);
		passed = (err == mips_Success) && (new_pc == value1);
	}
    
    else if(instruction == "jalr"){
    	uint32_t new_pc = test_get_jump_pc(cpu,mem);
    	if(dest == 0)
			mips_cpu_get_register(cpu, 31, &got);
		passed = (err == mips_Success) && (new_pc == value1) && (got == pc+8);
	}
    
    else if(instruction == "add"){
    	uint32_t sum = value1 + value2;
    	if((int32_t(value1) < 0 && int32_t(value2) < 0  && int32_t(sum) >= 0) | (int32_t(value1) >= 0 && int32_t(value2) >= 0  && int32_t(sum) < 0))
    		passed = (err == mips_ExceptionArithmeticOverflow) && (got == dest_init);
    	else
    		passed = (err == mips_Success) && ((got == sum) | ((got == 0) && (dest == 0)));    
    }
    
	else if(instruction == "addu")
		passed = (err == mips_Success) && (got==(value1 + value2)) | ((got == 0) && (dest == 0));
		
	else if(instruction == "sub"){
		uint32_t sum = value1 - value2;
    	if((int32_t(value1) >= 0 && int32_t(value2) < 0  && int32_t(sum) < 0) | (int32_t(value1) < 0 && int32_t(value2) >= 0  && int32_t(sum) >= 0))
    		passed = (err == mips_ExceptionArithmeticOverflow) && (got == dest_init);
    	else
    		passed = (err == mips_Success) && (got==(value1 - value2)) | ((got == 0) && (dest == 0));    
    }
	else if(instruction == "subu")
		passed = (err == mips_Success) && (got==(value1 - value2)) | ((got == 0) && (dest == 0));
	else if(instruction == "and")
		passed = (err == mips_Success) && (got==(value1 & value2)) | ((got == 0) && (dest == 0));
	else if(instruction == "or")
		passed = (err == mips_Success) && (got==(value1 | value2)) | ((got == 0) && (dest == 0));
	else if(instruction == "xor")
		passed = (err == mips_Success) && (got==(value1 xor value2)) | ((got == 0) && (dest == 0));
    else if(instruction == "sltu")
        passed = (err == mips_Success) && ((got == 0 && value1 > value2) | (got == 1 && value1 < value2));

	mips_test_end_test(testId, passed, NULL);



	return;
}

uint32_t i_type_create(std::string instruction, uint32_t src1, uint32_t dest, int16_t imm){

	uint32_t opcode;
	
	if(instruction == "bltz"){
		opcode = 0x1;
		dest = 0x0;
	}
	else if(instruction == "bgez"){
		opcode = 0x1;
		dest = 0x1;
	}
	else if(instruction == "bltzal"){
		opcode = 0x1;
		dest = 0x10;
	}	
	else if(instruction == "bgezal"){
		opcode = 0x1;
		dest = 0x11;
	}	
	else if(instruction == "beq")
		opcode = 0x4;
	else if(instruction == "bne")
		opcode = 0x5;
	else if(instruction == "blez")
		opcode = 0x6;
	else if(instruction == "bgtz")
		opcode = 0x7;
	else if(instruction == "addi")
		opcode = 0x8;
	else if(instruction == "addiu")
		opcode = 0x9;
	else if(instruction == "andi")
		opcode = 0xC;
	else if(instruction == "ori")
		opcode = 0xD;
	else if(instruction == "xori")
		opcode = 0xE;
	else if(instruction == "lui")
		opcode = 0xF;	
	else if(instruction == "lw")
		opcode = 0x23;
	else if(instruction == "sw")
		opcode = 0x2B;

	
	return (opcode << 26 | src1 << 21 | dest << 16 | (uint32_t(imm) & uint32_t(0xFFFF)));

}

void i_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t src1, uint32_t value1, uint32_t dest, int16_t imm, uint32_t value2){

	int testId=mips_test_begin_test(instruction.c_str());
	int passed=0;
	uint8_t bytes[4];

	mips_error err = mips_cpu_set_register(cpu, src1, value1);
	
	if((instruction == "beq") | (instruction == "bne"))
	err = mips_cpu_set_register(cpu, dest, value2);

	if(err==0)
	word_to_byte(i_type_create(instruction,src1,dest,imm),bytes);

	uint32_t pc, dest_init;
	err = mips_cpu_get_pc(cpu,&pc);

	if(err==0)
	err = mips_mem_write(mem,pc,4,bytes);

	if(src1 == 0)
		value1 = 0;
		
	if(err==0)
		err = mips_cpu_get_register(cpu, dest, &dest_init);
		
	if(err==0)
		err=mips_cpu_step(cpu);
			
	uint32_t got;
		mips_cpu_get_register(cpu, dest, &got);
	
	if(instruction == "bltz"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
				
		if(int32_t(value1) < 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "bgez"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
		if(int32_t(value1) >= 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "bltzal"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);

		mips_cpu_get_register(cpu, 31, &got);
		
		if(int32_t(value1) < 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2))) && (got == pc+8);
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8)) && (got == pc+8);
	}
	
	else if(instruction == "bgezal"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
		mips_cpu_get_register(cpu, 31, &got);
		
		if(int32_t(value1) >= 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2))) && (got == pc+8);
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8)) && (got == pc+8);
	}
	
	else if(instruction == "beq"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
		if(value1 == value2)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "bne"){
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
		if(value1 != value2)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "blez"){	
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
		if(int32_t(value1) <= 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "bgtz"){		
		uint32_t new_pc = test_get_jump_pc(cpu,mem);
						
		if(int32_t(value1) > 0)
			passed = (err == mips_Success) && (new_pc == (pc + 4 + (int32_t(imm) << 2)));
		else
			passed = (err == mips_Success) && (new_pc == (pc + 8));
	}
	
	else if(instruction == "addi"){
		uint32_t sum = value1 + uint32_t(imm);
    	if((int32_t(value1) < 0 && int32_t(imm) < 0  && int32_t(sum) >= 0) | (int32_t(value1) >= 0 && int32_t(imm) >= 0  && int32_t(sum) < 0)){
    		passed = (err == mips_ExceptionArithmeticOverflow) && (got == dest_init);
    	}
    	else
    		passed = (err == mips_Success) && ((got == sum) | ((got == 0) && (dest == 0)));
	}
	else if(instruction == "addiu"){
		passed = (err == mips_Success) && (got == (value1 + uint32_t(imm)));
	}
	else if(instruction == "andi"){
		passed = (err == mips_Success) && (got == (value1 & (uint32_t(imm) & uint32_t(0xFFFF))));
	}
	else if(instruction == "ori"){
		passed = (err == mips_Success) && (got == (value1 | (uint32_t(imm) & uint32_t(0xFFFF))));
	}
	else if(instruction == "xori"){
		passed = (err == mips_Success) && (got == (value1 xor (uint32_t(imm) & uint32_t(0xFFFF))));
	}
	
	else if(instruction == "lui"){
		passed = (err == mips_Success) && (got == uint32_t((int32_t(imm) << 16)));
	}
	
	else if(instruction == "lw"){
		err = mips_mem_read(mem,(value1 + int32_t(imm)),4,bytes);
		passed = (err == mips_Success) && (got == byte_to_word(bytes)) | ((got == 0) && (dest == 0));
	}
	else if(instruction == "sw"){
		uint8_t sw_bytes[4];
		
		err = mips_mem_read(mem,(value1 + imm),4,sw_bytes);
		
		uint32_t read_word = byte_to_word(sw_bytes);
		
		std::cout << "Address = "<< (value1 + imm) << std::endl;
		std::cout << "Read word = "<< read_word << std::endl;
		
		passed = (err == mips_Success) && (got == read_word);
	}

	mips_test_end_test(testId, passed, NULL);

	return;
}

uint32_t j_type_create(std::string instruction, uint32_t addr){

	uint32_t opcode;

	if(instruction == "j")
		opcode = 0x2;
	else if(instruction == "jal")
		opcode = 0x3;

	return (opcode << 26 | (addr & 0x03FFFFFF));
	
}

void j_ins(mips_cpu_h cpu, mips_mem_h mem, std::string instruction, uint32_t addr){

	int testId=mips_test_begin_test(instruction.c_str());
	int passed=0;
	uint8_t bytes[4];

	word_to_byte(j_type_create(instruction,addr),bytes);

	uint32_t pc;
	mips_error err = mips_cpu_get_pc(cpu,&pc);

	if(err==0)
		err = mips_mem_write(mem,pc,4,bytes);

	if(err==0)
		err=mips_cpu_step(cpu);
		
	uint32_t got;		
	mips_cpu_get_register(cpu, 31, &got);
	
	uint32_t new_pc = test_get_jump_pc(cpu,mem);
		
	if(instruction == "j"){
		passed = (err == mips_Success) && (new_pc == ((pc & 0xF0000000) | (addr << 2)));
	}
	else if(instruction == "jal"){
		passed = (err == mips_Success) && (new_pc == ((pc & 0xF0000000) | (addr << 2))) && (got == pc+8);
	}

	mips_test_end_test(testId, passed, NULL);

	return;
}

void test_set_zeroins(mips_cpu_h cpu, mips_mem_h mem){
	uint32_t temp_pc;
	mips_cpu_get_pc(cpu,&temp_pc);
	
	uint8_t zeroes[4];
	for(unsigned i = 0;i<4;i++){
		zeroes[i] = 0;
	
	}
	mips_mem_write(mem,temp_pc,4,zeroes);
}

uint32_t test_get_jump_pc(mips_cpu_h cpu, mips_mem_h mem){
	test_set_zeroins(cpu,mem);
	mips_cpu_step(cpu);
			
	uint32_t new_pc;
	mips_cpu_get_pc(cpu,&new_pc);
	
	return new_pc;
}
