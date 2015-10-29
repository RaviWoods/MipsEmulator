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
		}
	}
	

};

void r_type_test(test_instr instr, mips_cpu_h cpu_in) {
	
	int testId = mips_test_begin_test(instr.name.c_str());   
	mips_error e = mips_cpu_step(cpu_in);
	uint32_t got;
	uint32_t pc_new;
	e = mips_cpu_get_register(cpu_in, instr.src3, &got);
	e = mips_cpu_get_pc(cpu_in,&pc_new);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_step : failed.\n");
		exit(1);
	}
	int passed;
	if (got == instr.src3_success_data && instr.pc_aft == pc_new) {
		passed = 1;
	} else {
		passed = 0;
		cout << "failed, with inputs " << hex << instr.src1_data << " and " << hex << instr.src2_data << " and output " << hex << got << endl;
	}
	
    stringstream ss;
	ss << instr.name << "failed, with inputs " << instr.src1_data << " and  " << instr.src2_data << endl;
	
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
	r_type_test(addu1, cpu);
	
	test_instr addu2(4, "addu", 0, 0x01, 2, 0x1111111, 3, 0x1111111, cpu, mem);
	r_type_test(addu2, cpu);
	
	test_instr addu3(4, "addu", 4, 0x5555555, 2, 0x5555555, 0, 0x00, cpu, mem);
	r_type_test(addu3, cpu);
	
	//AND
	test_instr and1(4, "and", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0x0000F0F0, cpu, mem);
	r_type_test(and1, cpu);
	
	test_instr and2(4, "and", 0, 0x01, 2, 0x1111111, 3, 0x0, cpu, mem);
	r_type_test(and2, cpu);
	
	test_instr and3(4, "and", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	r_type_test(and3, cpu);
	
	//OR
	test_instr or1(4, "or", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0xF0F0FFFF, cpu, mem);
	r_type_test(or1, cpu);
	
	test_instr or2(4, "or", 0, 0xFFFFFFFF, 2, 0xF0F0F0F0, 3, 0xF0F0F0F0, cpu, mem);
	r_type_test(or2, cpu);
	
	test_instr or3(4, "or", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	r_type_test(or3, cpu);
	
	//XOR
	test_instr xor1(4, "xor", 10, 0xF0F0F0F0, 4, 0x0000FFFF, 31, 0xf0f00f0f, cpu, mem);
	r_type_test(xor1, cpu);
	
	test_instr xor2(4, "xor", 0, 0xFFFFFFFF, 2, 0xFFFFFFFF, 3, 0xFFFFFFFF, cpu, mem);
	r_type_test(xor2, cpu);
	
	test_instr xor3(4, "xor", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	r_type_test(xor3, cpu);
	
	//SUBU
	test_instr subu1(4, "subu", 10, 0x0000FFFF, 4, 0xFFFFFFFF, 31, 0x00010000, cpu, mem);
	r_type_test(subu1, cpu);
	
	test_instr subu2(4, "subu", 0, 0xFFFFFFFF, 2, 0xFFFFFFFF, 3, 0x1, cpu, mem);
	r_type_test(subu2, cpu);
	
	test_instr subu3(4, "subu", 4, 0x6666666, 2, 0x6666666, 0, 0x00, cpu, mem);
	r_type_test(subu3,cpu);
	
	//SLTU
	test_instr sltu1(4, "sltu", 10, 0x0000FFFF, 4, 0xFFFFFFFF, 31, 0x01, cpu, mem);
	r_type_test(sltu1, cpu);
	
	test_instr sltu2(4, "sltu", 28, 0xFFFFFFFF, 2, 0x0000FFFF, 3, 0x00, cpu, mem);
	r_type_test(sltu2, cpu);
	
	test_instr sltu3(4, "sltu", 4, 0xFFFFFFFF, 2, 0xFFFFFFFF, 5, 0x00, cpu, mem);
	r_type_test(sltu3,cpu);
	
	test_instr sltu4(4, "sltu", 0, 0x6666666, 5, 0x6666666, 5, 0x01, cpu, mem);
	r_type_test(sltu4,cpu);
	
	test_instr sltu5(4, "sltu", 4, 0x0000FFFF, 2, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	r_type_test(sltu5,cpu);
	
	//SLL
	test_instr sll1(4, "sll", 0x04, 0x00, 4, 0xFFFFFFFF, 31, 0xFFFFFFF0, cpu, mem);
	r_type_test(sll1, cpu);
	
	test_instr sll2(4, "sll", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	r_type_test(sll2, cpu);
	
	test_instr sll3(4, "sll", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	r_type_test(sll3, cpu);
	
	//SRL
	test_instr srl1(4, "srl", 0x04, 0x00, 4, 0xFFFFFFFF, 31, 0x0FFFFFFF, cpu, mem);
	r_type_test(srl1, cpu);
	
	test_instr srl2(4, "srl", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	r_type_test(srl2, cpu);
	
	test_instr srl3(4, "srl", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	r_type_test(srl3, cpu);
	
	//SRA
	test_instr sra1(4, "sra", 0x04, 0x00, 4, 0xF00000FF, 31, 0xFF00000F, cpu, mem);
	r_type_test(sra1, cpu);
	
	test_instr sra2(4, "sra", 0x08, 0x00, 4, 0x000000FFF, 31, 0x0000000F, cpu, mem);
	r_type_test(sra2, cpu);
	
	test_instr sra3(4, "sra", 0x04, 0x00, 0, 0xFFFFFFFF, 31, 0x00, cpu, mem);
	r_type_test(sra3, cpu);
	
	test_instr sra4(4, "sra", 0x04, 0x00, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	r_type_test(sra4, cpu);
	
	//SRAV
	test_instr srav1(4, "srav", 4, 0x04, 5, 0xF00000FF, 31, 0xFF00000F, cpu, mem);
	r_type_test(srav1, cpu);
	
	test_instr srav2(4, "srav", 7, 0x08, 6, 0x000000FFF, 31, 0x0000000F, cpu, mem);
	r_type_test(srav2, cpu);
	
	test_instr srav3(4, "srav", 0, 0x04, 5, 0x01, 31, 0x01, cpu, mem);
	r_type_test(srav3, cpu);
	
	test_instr srav4(4, "srav", 4, 0x04, 4, 0xFFFFFFFF, 0, 0x00, cpu, mem);
	r_type_test(srav4, cpu);
	
	//SLLV
	test_instr sllv1(4, "sllv", 4, 0xFFFFFFFF, 10, 0x11111111, 31, 0x80000000, cpu, mem);
	r_type_test(sllv1, cpu);
	
	test_instr sllv2(4, "sllv", 0, 0xFFFFFFFF, 3, 0xFFFFFFFF, 31, 0xFFFFFFFF, cpu, mem);
	r_type_test(sllv2, cpu);
	
	test_instr sllv3(4, "sllv", 4, 0xFFFFFFFF, 5, 0x11111111, 0, 0x00, cpu, mem);
	r_type_test(sllv3, cpu);
	
	//SRLV
	test_instr srlv1(4, "srlv", 4, 0x04, 10, 0xFFFFFFFF, 31, 0x0FFFFFFF, cpu, mem);
	r_type_test(srlv1, cpu);
	
	test_instr srlv2(4, "srlv", 0, 0xFFFFFFFF, 3, 0xFFFFFFFF, 31, 0xFFFFFFFF, cpu, mem);
	r_type_test(srlv2, cpu);
	
	test_instr srlv3(4, "srlv", 4, 0xFFFFFFFF, 5, 0x11111111, 0, 0x00, cpu, mem);
	r_type_test(srlv3, cpu);
	
	mips_test_end_suite();
    
    return 0;
}
