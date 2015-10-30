#include "headers_and_defines.h"

struct test_instr {
	string name;
	uint32_t whole;
	uint32_t opcode;
    uint32_t src1;
	uint32_t src1_data;
    uint32_t src2; 
	uint32_t src2_data;
   	uint32_t src3;
	uint32_t src3_success_data;
	uint32_t pc_bef;
	uint32_t pc_aft;
    uint32_t shift;
   	uint32_t function;
	uint32_t idata;
	int type; // R = 1, I = 2, J = 3 
	
	test_instr(uint32_t pc_diff, string name_in,  uint32_t src1_in, uint32_t src1_data_in, uint32_t src2_in, uint32_t src2_data_in, uint32_t src3_in, uint32_t src3_success_data_in, mips_cpu_h cpu_in, mips_mem_h mem_in ) {

		name = name_in;
		if (name_in == "addu") {
			function = 0x21;
			shift = 0;
			type = 1;
		} else if (name_in == "and") {
			function =  0x24;
			shift = 0;
			type = 1;
		} else if (name_in == "or") {
			function = 0x25;
			shift = 0;
			type = 1;
		} else if (name_in == "xor") {
			function =  0x26;
			shift = 0;
			type = 1;
		} else if (name_in == "sltu") {
			function = 0x2B;
			shift = 0;
			type = 1;
		} else if (name_in == "subu") {
			function =  0x23;
			shift = 0;
			type = 1;
		} else if (name_in == "sll") {
			shift = src1_in;
			src1_in = 0x00;
			function = 0x00;
			type = 1;
		} else if (name_in == "srl") {
			shift = src1_in;
			src1_in = 0x00;
			function =  0x02;
			type = 1;
		} else if (name_in == "sllv") {
			function = 0x04;
			shift = 0;
			type = 1;
		} else if (name_in == "srlv") {
			function = 0x06;
			shift = 0;
			type = 1;
		} else if (name_in == "sra") {
			shift = src1_in;
			src1_in = 0x00;
			function = 0x03;
			type = 1;
		} else if (name_in == "srav") {
			function = 0x07;
			shift = 0;
			type = 1;
		} else if (name_in == "add") {
			function = 0x20;
			shift = 0;
			type = 1;
		} else if (name_in == "sub") {
			function =  0x22;
			shift = 0;
			type = 1;
		} else if (name_in == "slt") {
			function = 0x2A;
			shift = 0;
			type = 1;
		} else if (name_in == "andi") {
			opcode = 0x0C;
			shift = 0;
			type = 2;
		} else if (name_in == "xori") {
			opcode = 0x0E;
			shift = 0;
			type = 2;
		} else if (name_in == "ori") {
			opcode = 0x0D;
			shift = 0;
			type = 2;
		} else if (name_in == "addiu") {
			opcode = 0x09;
			shift = 0;
			type = 2;
		} else if (name_in == "addi") {
			opcode = 0x08;
			shift = 0;
			type = 2;
		} else if (name_in == "sltiu") {
			opcode = 0x0B;
			shift = 0;
			type = 2;
		} else if (name_in == "slti") {
			opcode = 0x0A;
			shift = 0;
			type = 2;
		} else if (name_in == "lw") {
			opcode = 0x23;
			shift = 0;
			type = 2;
		} else if (name_in == "lui") {
			opcode = 0x0F;
			src1_in = 0;
			src2_in = 0;
			shift = 0;
			type = 2;
		} else if (name_in == "sw") {
			opcode = 0x2B;
			shift = 0;
			type = 2;
		} 
		
		if (type == 1) {
			opcode = 0;
			src1 = src1_in;
			src1_data = src1_data_in;
			src2 = src2_in;
			src2_data = src2_data_in;
			src3 = src3_in;
			src3_success_data = src3_success_data_in;
			opcode = 0;


			mips_error e = mips_cpu_set_register(cpu_in, src1, src1_data);
			e = mips_cpu_set_register(cpu_in, src2, src2_data);
			e = mips_cpu_get_pc(cpu_in,&pc_bef);
			pc_aft = pc_bef + pc_diff;
			
			uint32_t whole =
				(src1 << 21) // srca = r4
				|
				(src2 << 16) // srcb = r5
				|
				(src3 << 11) // dst = r3
				|
				(shift << 6) // shift = 0
				|
				(function << 0);
    
			uint8_t buffer[4];
			buffer[0]=(whole>>24)&0xFF;
			buffer[1]=(whole>>16)&0xFF;
			buffer[2]=(whole>>8)&0xFF;
			buffer[3]=(whole>>0)&0xFF;

			e = mips_mem_write(
				mem_in,	        //!< Handle to target memory
				pc_bef,	            //!< Byte address to start transaction at
				4,	            //!< Number of bytes to transfer
				buffer	        //!< Receives the target bytes
			);
			
			if(e!=mips_Success){
				fprintf(stderr, "mips_cpu_step : failed.\n");
				exit(1);
			}
		} else if (type == 2) {
			src1 = src1_in;
			src1_data = src1_data_in;
			idata = src2_data_in;
			src3 = src3_in;
			src3_success_data = src3_success_data_in;


			mips_error e = mips_cpu_set_register(cpu_in, src1, src1_data);
			e = mips_cpu_get_pc(cpu_in,&pc_bef);
			pc_aft = pc_bef + pc_diff;
			
			uint32_t whole =
				((opcode  & 0x2F) << 26)
				|
				((src1  & 0x1F) << 21)
				|
				((src3  & 0x1F) << 16) 
				|
				(idata & 0xFFFF);
    
			uint8_t buffer[4];
			buffer[0]=(whole>>24)&0xFF;
			buffer[1]=(whole>>16)&0xFF;
			buffer[2]=(whole>>8)&0xFF;
			buffer[3]=(whole>>0)&0xFF;
			e = mips_mem_write(
				mem_in,	        //!< Handle to target memory
				pc_bef,	            //!< Byte address to start transaction at
				4,	            //!< Number of bytes to transfer
				buffer	        //!< Receives the target bytes
			);
			
			if(e!=mips_Success){
				fprintf(stderr, "mips_cpu_step : failed.\n");
				exit(1);
			}
		}
	}
	

};

void basic_inst_test(test_instr instr, mips_cpu_h cpu_in, mips_error test_error_in) {
	
	int testId = mips_test_begin_test(instr.name.c_str());  
	mips_error test_error;
	mips_error e = mips_cpu_step(cpu_in);
	test_error = e;
	e = mips_Success;
	uint32_t got;
	uint32_t pc_new;
	e = mips_cpu_get_register(cpu_in, instr.src3, &got);
	e = mips_cpu_get_pc(cpu_in,&pc_new);

	int passed;
	
	cout << instr.name << " " << pc_new<< endl;
	if (test_error_in != mips_Success) {
		if (test_error == test_error_in) {
			if (instr.pc_aft == pc_new) {
				e = mips_cpu_reset(cpu_in);
				if (e != mips_Success) {
					cout << "ERROR" << endl;
				}
				passed = 1;
			}
		} else if (test_error == mips_Success) {
			cout << "BAD SUCCESS" << endl;
			passed = 0;
		} else {
			fprintf(stderr, "mips_cpu_step : failed.\n");
			exit(1);
		}
	} else if (got == instr.src3_success_data && instr.pc_aft == pc_new) {
		passed = 1;
	} else {
		passed = 0;
		cout << instr.name << " failed, with pc " << pc_new << " and "  << " which shouldve been "  << instr.pc_aft << endl;
		cout << instr.name << " failed, with inputs " << hex << instr.src1_data << " and " << hex << instr.idata << " and output " << hex << got << endl;
	}
	
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu : failed.\n");
		exit(1);
	}
	
    stringstream ss;
	ss << instr.name << "failed, with inputs " << instr.src1_data << " and  " << instr.src2_data << endl;
	
	mips_test_end_test(testId, passed, ss.str().c_str());
}

void lw_test(test_instr instr, mips_cpu_h cpu_in, mips_mem_h mem_in, mips_error test_error_in) {
	
	int testId = mips_test_begin_test(instr.name.c_str());  
	mips_error test_error;
	uint32_t word = instr.src3_success_data;
	
	uint8_t buffer[4];
	buffer[0]=(word>>24)&0xFF;
	buffer[1]=(word>>16)&0xFF;
	buffer[2]=(word>>8)&0xFF;
	buffer[3]=(word>>0)&0xFF;
	mips_error e = mips_mem_write(
		mem_in,	        //!< Handle to target memory
		instr.src3,	            //!< Byte address to start transaction at
		4,	            //!< Number of bytes to transfer
		buffer	        //!< Receives the target bytes
	);
	
	e = mips_cpu_step(cpu_in);
	test_error = e;
	e = mips_Success;
	uint32_t got;
	uint32_t pc_new;
	e = mips_cpu_get_register(cpu_in, instr.src3, &got);
	e = mips_cpu_get_pc(cpu_in,&pc_new);

	int passed;


			
	cout << instr.name << " " << pc_new<< endl;
	if (test_error_in != mips_Success) {
		if (test_error == test_error_in) {
			if (instr.pc_aft == pc_new) {
				e = mips_cpu_reset(cpu_in);
				if (e != mips_Success) {
					cout << "ERROR" << endl;
				}
				passed = 1;
			}
		} else if (test_error == mips_Success) {
			cout << "BAD SUCCESS" << endl;
			passed = 0;
		} else {
			fprintf(stderr, "mips_cpu_step : failed.\n");
			exit(1);
		}
	} else if (got == instr.src3_success_data && instr.pc_aft == pc_new) {
		passed = 1;
	} else {
		passed = 0;
		cout << instr.name << " failed, with pc " << pc_new << " and "  << " which shouldve been "  << instr.pc_aft << endl;
		cout << instr.name << " failed, with inputs " << hex << instr.src1_data << " and " << hex << instr.idata << " and output " << hex << got << endl;
	}
	
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu : failed.\n");
		exit(1);
	}
	
    stringstream ss;
	ss << instr.name << "failed, with inputs " << instr.src1_data << " and  " << instr.idata << endl;
	
	mips_test_end_test(testId, passed, ss.str().c_str());
}

void sw_test(test_instr instr, mips_cpu_h cpu_in, mips_mem_h mem_in, mips_error test_error_in) {
	
	int testId = mips_test_begin_test(instr.name.c_str());  
	mips_error test_error;
	mips_error e = mips_cpu_set_register(cpu_in, instr.src3, instr.src3_success_data);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu : failed.\n");
		exit(1);
	}
	e = mips_cpu_step(cpu_in);
	test_error = e;
	e = mips_Success;
	uint32_t pc_new;
	
	e = mips_cpu_get_pc(cpu_in,&pc_new);
	
	int passed;

	if(e!=mips_Success){
		
		fprintf(stderr, "mips_cpu : failed.\n");
		exit(1);
	}
	
	
	uint8_t buffer[4];
	e = mips_mem_read(
		mem_in,	        //!< Handle to target memory
		instr.src3,	            //!< Byte address to start transaction at
		4,	            //!< Number of bytes to transfer
		buffer	        //!< Receives the target bytes
	);
	if(e!=mips_Success){
		cout << "ARGH" << endl;
		fprintf(stderr, "mips_cpu : failed.\n");
		exit(1);
	}
	
	uint32_t got = 
        (((uint32_t)buffer[0])<<24)
        |
        (((uint32_t)buffer[1])<<16)
        |
        (((uint32_t)buffer[2])<<8)
        |
        (((uint32_t)buffer[3])<<0);
			
	cout << instr.name << " " << pc_new<< endl;
	if (test_error_in != mips_Success) {
		if (test_error == test_error_in) {
			if (instr.pc_aft == pc_new) {
				e = mips_cpu_reset(cpu_in);
				if(e!=mips_Success){
					fprintf(stderr, "mips_cpu : failed.\n");
					exit(1);
				}
				passed = 1;
			}
		} else if (test_error == mips_Success) {
			cout << "BAD SUCCESS" << endl;
			passed = 0;
		} else {
			fprintf(stderr, "mips_cpu_step : failed.\n");
			exit(1);
		}
	} else if (got == instr.src3_success_data && instr.pc_aft == pc_new) {
		passed = 1;
	} else {
		passed = 0;
		cout << instr.name << " failed, with pc " << pc_new << " and "  << " which shouldve been "  << instr.pc_aft << endl;
		cout << instr.name << " failed, with inputs " << hex << instr.src1_data << " and " << hex << instr.idata << " and output " << hex << got << endl;
	}
	

    stringstream ss;
	ss << instr.name << "failed, with inputs " << instr.src1_data << " and  " << instr.idata << endl;
	
	mips_test_end_test(testId, passed, ss.str().c_str());
}


int main()
{
    mips_mem_h mem=mips_mem_create_ram(4096, 4);
    
    mips_cpu_h cpu=mips_cpu_create(mem);
    
    mips_error e = mips_cpu_set_debug_level(cpu, DEBUG_LEVEL, stderr);
    if(e != mips_Success){
        fprintf(stderr, "mips_cpu_set_debug_level : failed.\n");
        exit(1);
    }
    
    
    mips_test_begin_suite();
	
	//ADDU
	test_instr addu1(4, "addu", 10, 0xFFFFFFFF, 4, 0xFFFFFFFF, 31, 0xFFFFFFFE, cpu, mem);
	basic_inst_test(addu1, cpu,  mips_Success);
	
	test_instr addu2(4, "addu", 0, 0x01, 2, 0x1111111, 3, 0x1111111, cpu, mem);
	basic_inst_test(addu2, cpu,  mips_Success);
	
	test_instr addu3(4, "addu", 4, 0x5555555, 2, 0x5555555, 0, 0x00, cpu, mem);
	basic_inst_test(addu3, cpu,  mips_Success);
	
	//AND
	test_instr and1(4, "and", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0x0000F0F0, cpu, mem);
	basic_inst_test(and1, cpu,  mips_Success);
	
	test_instr and2(4, "and", 0, 0x01, 2, 0x1111111, 3, 0x0, cpu, mem);
	basic_inst_test(and2, cpu,  mips_Success);
	
	test_instr and3(4, "and", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	basic_inst_test(and3, cpu,  mips_Success);
	
	//OR
	test_instr or1(4, "or", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0xF0F0FFFF, cpu, mem);
	basic_inst_test(or1, cpu,  mips_Success);
	
	test_instr or2(4, "or", 0, 0xFFFFFFFF, 2, 0xF0F0F0F0, 3, 0xF0F0F0F0, cpu, mem);
	basic_inst_test(or2, cpu,  mips_Success);
	
	test_instr or3(4, "or", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	basic_inst_test(or3, cpu,  mips_Success);
	
	//XOR
	test_instr xor1(4, "xor", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0xf0f00f0f, cpu, mem);
	basic_inst_test(xor1, cpu,  mips_Success);
	
	test_instr xor2(4, "xor", 0, 0xFFFFFFFF, 2, 0xFFFFFFFF, 3, 0xFFFFFFFF, cpu, mem);
	basic_inst_test(xor2, cpu,  mips_Success);
	
	test_instr xor3(4, "xor", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	basic_inst_test(xor3, cpu,  mips_Success);
	
	//SUBU
	test_instr subu1(4, "subu", 10, 0x0000FFFF, 4, 0xFFFFFFFF, 31, 0x00010000, cpu, mem);
	basic_inst_test(subu1, cpu,  mips_Success);
	
	test_instr subu2(4, "subu", 0, 0xFFFFFFFF, 2, 0xFFFFFFFF, 3, 0x1, cpu, mem);
	basic_inst_test(subu2, cpu,  mips_Success);
	
	test_instr subu3(4, "subu", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	basic_inst_test(subu3, cpu,  mips_Success);
	
	//SLTU
	test_instr sltu1(4, "sltu", 10, 0x0000FFFF, 4, 0xFFFFFFFF, 31, 0x01, cpu, mem);
	basic_inst_test(sltu1, cpu,  mips_Success);
	
	test_instr sltu2(4, "sltu", 28, 0xFFFFFFFF, 2, 0x0000FFFF, 3, 0x00, cpu, mem);
	basic_inst_test(sltu2, cpu,  mips_Success);
	
	test_instr sltu3(4, "sltu", 4, 0xFFFFFFFF, 2, 0xFFFFFFFF, 5, 0x00, cpu, mem);
	basic_inst_test(sltu3, cpu,  mips_Success);
	
	test_instr sltu4(4, "sltu", 0, 0x6666666, 5, 0x6666666, 5, 0x01, cpu, mem);
	basic_inst_test(sltu4, cpu,  mips_Success);
	
	test_instr sltu5(4, "sltu", 4, 0x0000FFFF, 2, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(sltu5, cpu,  mips_Success);
	
	//SLL
	test_instr sll1(4, "sll", 0x04, 0x00, 4, 0xFFFFFFFF, 31, 0xFFFFFFF0, cpu, mem);
	basic_inst_test(sll1, cpu,  mips_Success);
	
	test_instr sll2(4, "sll", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	basic_inst_test(sll2, cpu,  mips_Success);
	
	test_instr sll3(4, "sll", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(sll3, cpu,  mips_Success);
	
	//SRL
	test_instr srl1(4, "srl", 0x04, 0x00, 4, 0xFFFFFFFF, 31, 0x0FFFFFFF, cpu, mem);
	basic_inst_test(srl1, cpu,  mips_Success);
	
	test_instr srl2(4, "srl", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	basic_inst_test(srl2, cpu,  mips_Success);
	
	test_instr srl3(4, "srl", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(srl3, cpu,  mips_Success);
	
	//SRA
	test_instr sra1(4, "sra", 0x04, 0x00, 4, 0xF00000FF, 31, 0xFF00000F, cpu, mem);
	basic_inst_test(sra1, cpu,  mips_Success);
	
	test_instr sra2(4, "sra", 0x08, 0x00, 4, 0x000000FFF, 31, 0x0000000F, cpu, mem);
	basic_inst_test(sra2, cpu,  mips_Success);
	
	test_instr sra3(4, "sra", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	basic_inst_test(sra3, cpu,  mips_Success);
	
	test_instr sra4(4, "sra", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(sra4, cpu,  mips_Success);
	
	//SRAV
	test_instr srav1(4, "srav", 4, 0x04, 5, 0xF00000FF, 31, 0xFF00000F, cpu, mem);
	basic_inst_test(srav1, cpu,  mips_Success);
	
	test_instr srav2(4, "srav", 7, 0x08, 6, 0x000000FFF, 31, 0x0000000F, cpu, mem);
	basic_inst_test(srav2, cpu,  mips_Success);
	
	test_instr srav3(4, "srav", 0, 0x04, 5, 0x01, 31, 0x01, cpu, mem);
	basic_inst_test(srav3, cpu,  mips_Success);
	
	test_instr srav4(4, "srav", 4, 0x04, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(srav4, cpu,  mips_Success);
	
	//SLLV
	test_instr sllv1(4, "sllv", 4, 0xFFFFFFFF, 10, 0x11111111, 31, 0x80000000, cpu, mem);
	basic_inst_test(sllv1, cpu,  mips_Success);
	
	test_instr sllv2(4, "sllv", 0, 0xFFFFFFFF, 3, 0xFFFFFFFF, 31, 0xFFFFFFFF, cpu, mem);
	basic_inst_test(sllv2, cpu,  mips_Success);
	
	test_instr sllv3(4, "sllv", 4, 0xFFFFFFFF, 5, 0x11111111, 0, 0x00, cpu, mem);
	basic_inst_test(sllv3, cpu,  mips_Success);
	
	//SRLV
	test_instr srlv1(4, "srlv", 4, 0x04, 10, 0xFFFFFFFF, 31, 0x0FFFFFFF, cpu, mem);
	basic_inst_test(srlv1, cpu,  mips_Success);
	
	test_instr srlv2(4, "srlv", 0, 0xFFFFFFFF, 3, 0xFFFFFFFF, 31, 0xFFFFFFFF, cpu, mem);
	basic_inst_test(srlv2, cpu,  mips_Success);
	
	test_instr srlv3(4, "srlv", 4, 0xFFFFFFFF, 5, 0x11111111, 0, 0x00, cpu, mem);
	basic_inst_test(srlv3, cpu,  mips_Success);
	
	//XORI
	test_instr xori1(4, "xori", 10, 0xF0F0F0F0, 0, 0xFFFF, 31, 0xf0f00f0f, cpu, mem);
	basic_inst_test(xori1, cpu,  mips_Success);
	
	test_instr xori2(4, "xori", 0, 0xFFFFFFFF, 0, 0xFFFF, 3, 0x0000FFFF, cpu, mem);
	basic_inst_test(xori2, cpu,  mips_Success);
	
	test_instr xori3(4, "xori", 4, 0x6666666, 0, 0x6666, 0, 0x00, cpu, mem);
	basic_inst_test(xori3, cpu,  mips_Success);
	
	//ADDIU
	test_instr addiu1(4, "addiu", 10, 0xFFFFFFFF, 0, 0xFFFF, 31, 0xFFFFFFFE, cpu, mem);
	basic_inst_test(addiu1, cpu,  mips_Success);
	
	test_instr addiu2(4, "addiu", 10, 0xFFFFFFFF, 0, 0x7FFF, 31, 0x00007ffe, cpu, mem);
	basic_inst_test(addiu2, cpu,  mips_Success);
	
	test_instr addiu3(4, "addiu", 0, 0x01, 0, 0x1111, 3, 0x1111, cpu, mem);
	basic_inst_test(addiu3, cpu,  mips_Success);
	
	test_instr addiu4(4, "addiu", 4, 0x5555555, 0, 0x5555, 0, 0x00, cpu, mem);
	basic_inst_test(addiu4, cpu,  mips_Success);
	
	//ANDI
	test_instr andi1(4, "andi", 10, 0xF0F0F0F0, 0, 0xFFFF, 31, 0x0000F0F0, cpu, mem);
	basic_inst_test(andi1, cpu,  mips_Success);
	
	test_instr andi2(4, "andi", 0, 0x01, 0, 0x1111, 3, 0x0, cpu, mem);
	basic_inst_test(andi2, cpu,  mips_Success);
	
	test_instr andi3(4, "andi", 4, 0x6666666, 0, 0x6666, 0, 0x00, cpu, mem);
	basic_inst_test(andi3, cpu,  mips_Success);
	
	//ORI
	test_instr ori1(4, "ori", 10, 0xF0F0F0F0, 0, 0xFFFF, 31, 0xF0F0FFFF, cpu, mem);
	basic_inst_test(ori1, cpu,  mips_Success);
	
	test_instr ori2(4, "ori", 0, 0xFFFFFFFF, 0, 0xF0F0, 3, 0x0000F0F0, cpu, mem);
	basic_inst_test(ori2, cpu,  mips_Success);
	
	test_instr ori3(4, "ori", 4, 0x6666666, 0, 0x6666, 0, 0x00, cpu, mem);
	basic_inst_test(ori3, cpu,  mips_Success);
	
	//sltiu
	test_instr sltiu1(4, "sltiu", 10, 0x0000FFFF, 0, 0xFFFF, 31, 0x01, cpu, mem);
	basic_inst_test(sltiu1, cpu,  mips_Success);
	
	test_instr sltiu2(4, "sltiu", 28, 0xFFFF, 0, 0x7FFF, 3, 0x00, cpu, mem);
	basic_inst_test(sltiu2, cpu,  mips_Success);
	
	test_instr sltiu3(4, "sltiu", 28, 0xFFFFFFFF, 0, 0xFFFF, 3, 0x00, cpu, mem);
	basic_inst_test(sltiu3, cpu,  mips_Success);
	
	test_instr sltiu4(4, "sltiu", 0, 0xFFFFFFFF, 0, 0x0001, 5, 0x01, cpu, mem);
	basic_inst_test(sltiu4, cpu,  mips_Success);
	
	test_instr sltiu5(4, "sltiu", 4, 0x0000FFFF, 0, 0xFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(sltiu5, cpu,  mips_Success);
    
	//ADD
	test_instr add1(0, "add", 4, 0x7FFFFFFF, 5, 0x7FFFFFFF, 6, 0x00, cpu, mem);
	basic_inst_test(add1, cpu,  mips_ExceptionArithmeticOverflow);	
	
	test_instr add2(0, "add", 4, 0x80000000, 5, 0x80000000, 6, 0x00, cpu, mem);
	basic_inst_test(add2, cpu,  mips_ExceptionArithmeticOverflow);

	test_instr add3(4, "add", 4, 0xFFFFFFFF, 5, 0x2, 6, 0x01, cpu, mem);
	basic_inst_test(add3, cpu,  mips_Success);
	
	test_instr add4(4, "add", 0, 0x01, 2, 0x1111111, 3, 0x1111111, cpu, mem);
	basic_inst_test(add4, cpu,  mips_Success);
	
	test_instr add5(4, "add", 4, 0x5555555, 2, 0x5555555, 0, 0x00, cpu, mem);
	basic_inst_test(add5, cpu,  mips_Success);
	
	//SUBU
	test_instr sub1(0, "sub", 10, 0x80000000, 4, 0x01, 31, 0x00, cpu, mem);
	basic_inst_test(sub1, cpu,  mips_ExceptionArithmeticOverflow);
	
	test_instr sub2(0, "sub", 4, 0x7FFFFFFF, 2, 0xFFFFFFFF, 3, 0x00, cpu, mem);
	basic_inst_test(sub2, cpu,  mips_ExceptionArithmeticOverflow);
	
	test_instr sub3(4, "sub", 4, 0x02, 2, 0x01, 3, 0x01, cpu, mem);
	basic_inst_test(sub3, cpu,  mips_Success);
	
	test_instr sub4(4, "sub", 5, 0x01, 8, 0xFFFFFFFF, 11, 0x02, cpu, mem);
	basic_inst_test(sub4, cpu,  mips_Success);
	
	test_instr sub5(4, "sub", 6, 0xFFFFFFFF, 9, 0x01, 12, 0xFFFFFFFE, cpu, mem);
	basic_inst_test(sub5, cpu,  mips_Success);
	
	test_instr sub6(4, "sub", 7, 0xFFFFFFFE, 10, 0xFFFFFFFF, 13, 0xFFFFFFFF, cpu, mem);
	basic_inst_test(sub6, cpu,  mips_Success);
	
	test_instr sub7(4, "sub", 15, 0xF, 0, 0xFFFFFFFF, 16, 0xF, cpu, mem);
	basic_inst_test(sub7, cpu,  mips_Success);
	
	test_instr sub8(4, "sub", 17, 0xFFFFFFFE, 10, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	basic_inst_test(sub8, cpu,  mips_Success);
	
	//SLT
	test_instr slt1(4, "slt", 10, 0x01, 4, 0x80000000, 31, 0x00, cpu, mem);
	basic_inst_test(slt1, cpu,  mips_Success);
	
	test_instr slt2(4, "slt", 28, 0xFFFFFFFF, 2, 0x01, 3, 0x01, cpu, mem);
	basic_inst_test(slt2, cpu,  mips_Success);
	
	test_instr slt3(4, "slt", 4, 0x01, 2, 0xFFFFFFFF, 5, 0x00, cpu, mem);
	basic_inst_test(slt3, cpu,  mips_Success);
	
	test_instr slt4(4, "slt", 7, 0xFFFFFFFF, 5, 0xFFFFFFFE, 5, 0x00, cpu, mem);
	basic_inst_test(slt4, cpu,  mips_Success);
	
	test_instr slt5(4, "slt", 10, 0x01, 1, 0x01, 5, 0x00, cpu, mem);
	basic_inst_test(slt5, cpu,  mips_Success);
	
	test_instr slt6(4, "slt", 2, 0x0, 0, 0x2, 12, 0x00, cpu, mem);
	basic_inst_test(slt6, cpu,  mips_Success);
	
	test_instr slt7(4, "slt", 5, 0x0, 11, 0x2, 0, 0x00, cpu, mem);
	basic_inst_test(slt7, cpu,  mips_Success);
	
	//SLTI
	
	test_instr slti1(4, "slti", 7, 0xFFFFFFFF, 0, 0x7FFE, 5, 0x01, cpu, mem);
	basic_inst_test(slti1, cpu,  mips_Success);
	
	test_instr slti2(4, "slti", 28, 0xFFFFFFFF, 0, 0x01, 3, 0x01, cpu, mem);
	basic_inst_test(slti2, cpu,  mips_Success);
	
	test_instr slti3(4, "slti", 4, 0x01, 0, 0xFFFF, 5, 0x00, cpu, mem);
	basic_inst_test(slti3, cpu,  mips_Success);
	
	test_instr slti4(4, "slti", 7, 0xFFFFFFFF, 0, 0xFFFE, 5, 0x00, cpu, mem);
	basic_inst_test(slti4, cpu,  mips_Success);
	
	test_instr slti5(4, "slti", 10, 0x01, 0, 0x01, 5, 0x00, cpu, mem);
	basic_inst_test(slti5, cpu,  mips_Success);
	
	test_instr slti6(4, "slti", 0, 0x2, 0, 0x2, 12, 0x01, cpu, mem);
	basic_inst_test(slti6, cpu,  mips_Success);
	
	test_instr slti7(4, "slti", 5, 0x0, 0, 0x2, 0, 0x00, cpu, mem);
	basic_inst_test(slti7, cpu,  mips_Success);
	
	//ADDI
	
	test_instr addi1(0, "addi", 4, 0x7FFFFFFF, 10, 0x7FFF, 3, 0x00, cpu, mem);
	basic_inst_test(addi1, cpu,  mips_ExceptionArithmeticOverflow);
	
	test_instr addi2(0, "addi", 4, 0x80000000, 0, 0xFFFF, 6, 0x00, cpu, mem);
	basic_inst_test(addi2, cpu,  mips_ExceptionArithmeticOverflow);
	
	test_instr addi3(4, "addi", 4, 0xFFFFFFFF, 0, 0x2, 6, 0x01, cpu, mem);
	basic_inst_test(addi3, cpu,  mips_Success);
	
	test_instr addi4(4, "addi", 0, 0x01, 0, 0x1111, 3, 0x1111, cpu, mem);
	basic_inst_test(addi4, cpu,  mips_Success);
	
	test_instr addi5(4, "addi", 4, 0x5555555, 0, 0x5555, 0, 0x00, cpu, mem);
	basic_inst_test(addi5, cpu,  mips_Success);
	
	test_instr addi6(4, "addi", 4, 0x7FFFFFFF, 0, 0xFFFF, 3, 0x7FFFFFFE, cpu, mem);
	basic_inst_test(addi6, cpu,  mips_Success);
	
	test_instr addi7(4, "addi", 4, 0xF0000000, 0, 0x7FFF, 6, 0xF0007FFF, cpu, mem);
	basic_inst_test(addi7, cpu,  mips_Success);
	
	//LUI
	test_instr lui1(4, "lui", 0, 0x00, 0, 0xFFFF, 6, 0xFFFF0000, cpu, mem);
	basic_inst_test(lui1, cpu,  mips_Success);
	
	//LW
	test_instr lw1(4, "lw", 2, 0x4, 0, 0x04, 8, 0x12345678, cpu, mem);
	lw_test(lw1, cpu, mem, mips_Success);
	
	test_instr lw2(0, "lw", 2, 0x4, 0, 0x02, 6, 0x12345678, cpu, mem);
	lw_test(lw2, cpu, mem, mips_ExceptionInvalidAlignment);
	
	test_instr lw3(4, "lw", 2, 0x5, 0, 0xFFFFFFFF, 4, 0x12345678, cpu, mem);
	lw_test(lw3, cpu, mem, mips_Success);
	
	//SW
	test_instr sw1(4, "sw", 2, 0x4, 0, 0x04, 8, 0x12345678, cpu, mem);
	sw_test(sw1, cpu, mem, mips_Success);
	
	test_instr sw2(0, "sw", 2, 0x4, 0, 0x02, 4, 0x12345678, cpu, mem);
	sw_test(sw2, cpu, mem, mips_ExceptionInvalidAlignment);
	
	test_instr sw3(4, "sw", 2, 0x5, 0, 0xFFFFFFFF, 4, 0x12345678, cpu, mem);
	sw_test(sw3, cpu, mem, mips_Success);
	
	
	
	mips_test_end_suite();
    
    return 0;
}
