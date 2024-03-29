#include "headers_and_defines.h"
//
int rtestsetup(string name, unsigned srca, unsigned srcb, unsigned long dst, unsigned long shift, mips_mem_h mem, mips_cpu_h cpu) {
	 
	int subtype;
    	unsigned long function;
	
	if (name == "addu") {
		function = 0x21;
		subtype = 1;
	} else if (name == "and") {
		function = 0x24;
		subtype = 1;
	} else if (name == "or") {
		function = 0x25;
		subtype = 1;
	} else if (name == "xor") {
		function = 0x26;
		subtype = 1;
	} else if (name == "sltu") {
		function = 0x28;
		subtype = 1;
	}
    	// 1 - Setup an instruction in ram
    	// addu r3, r4, r5
    uint32_t instr =
        (srca << 21) // srca = r4
        |
        (srcb << 16) // srcb = r5
        |
        (dst << 11) // dst = r3
        |
        (shift << 6) // shift = 0
        |
        (function << 0);
    
    uint8_t buffer[4];
    buffer[0]=(instr>>24)&0xFF;
    buffer[1]=(instr>>16)&0xFF;
    buffer[2]=(instr>>8)&0xFF;
    buffer[3]=(instr>>0)&0xFF;

    uint32_t* pc;
    mips_error e = mips_cpu_get_pc(cpu,pc);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_get_pc : failed.\n");
		exit(1);
	} 
    e = mips_mem_write(
        mem,	        //!< Handle to target memory
		*pc,	            //!< Byte address to start transaction at
        4,	            //!< Number of bytes to transfer
        buffer	        //!< Receives the target bytes
    );
    if(e!=mips_Success){
        fprintf(stderr, "mips_mem_write : failed.\n");
        exit(1);
    }
	return subtype;
}

int itestsetup(string name, unsigned srca, unsigned srcb, uint16_t ioffset, mips_mem_h mem, mips_cpu_h cpu) {
	 
	int subtype;
	uint32_t opcode;
	if (name == "beq") {
		opcode = 0x04;
		subtype = 1;
	} 
    	// 1 - Setup an instruction in ram
    	// addu r3, r4, r5
    uint32_t instr =
		(opcode << 26)
		|
        (srca << 21) // srca = r4
        |
        (srcb << 16) // srcb = r5
        |
        (ioffset << 0);
    
    uint8_t buffer[4];
    buffer[0]=(instr>>24)&0xFF;
    buffer[1]=(instr>>16)&0xFF;
    buffer[2]=(instr>>8)&0xFF;
    buffer[3]=(instr>>0)&0xFF;

    uint32_t* pc;
    mips_error e = mips_cpu_get_pc(cpu,pc);
    e = mips_mem_write(
        mem,	        //!< Handle to target memory
		*pc,	            //!< Byte address to start transaction at
        4,	            //!< Number of bytes to transfer
        buffer	        //!< Receives the target bytes
    );
    if(e!=mips_Success){
        fprintf(stderr, "mips_mem_write : failed.\n");
        exit(1);
    }
	return subtype;
}


void subtype2(int testId, string name, unsigned indexa, uint32_t valuea, unsigned indexb, uint32_t valueb, uint16_t success_pc, mips_cpu_h cpu) {

	mips_error e = mips_cpu_set_register(cpu, indexa, valuea);
	e = mips_cpu_set_register(cpu, indexb, valueb);
	

	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_get_pc : failed.\n");
		exit(1);
	}  
	
	e = mips_cpu_step(cpu);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_step : failed.\n");
		exit(1);
	}
	
	uint32_t pc1;
	e = mips_cpu_get_pc(cpu,&pc1);
	cout << "pc1 = " << hex << pc1 << endl;
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_get_pc : failed.\n");
		exit(1);
	}  
	
	e = mips_cpu_step(cpu);
	if(e != mips_Success){
		fprintf(stderr, "mips_cpu_step : failed.\n");
		exit(1);
	}
	
	uint32_t pc2;
	e = mips_cpu_get_pc(cpu,&pc2);
	cout << "pc2 = " << hex << pc2 << endl;
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_get_pc : failed.\n");
		exit(1);
	} 
	
	uint32_t got;
	int passed = 0;
	
	uint16_t unsig_success_pc;
	
	bool negative = sig_to_unsig(success_pc,&unsig_success_pc);
	if (negative) {
		//Gone backwards
		if ((pc1-pc2) == unsig_success_pc) {
			passed = 1;
		}
	} else {
		if ((pc2-pc1) == unsig_success_pc) {
			passed = 1;
		}
	}
	
	cout << "success_pc = " << hex << success_pc << endl;

    stringstream ss;
	ss << name << "failed, with inputs " << valuea << " and  " << valueb << endl;
	e = mips_cpu_set_pc(cpu,pc1);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_get_pc : failed.\n");
		exit(1);
	} 
	mips_test_end_test(testId, passed, ss.str().c_str());
}

void subtype1(int testId, string name, unsigned indexa, uint32_t valuea, unsigned indexb, uint32_t valueb, unsigned dst, uint32_t success_value, mips_cpu_h cpu) {
	mips_error e = mips_cpu_set_register(cpu, indexa, valuea);
	e = mips_cpu_set_register(cpu, indexb, valueb);
    
	e = mips_cpu_step(cpu);
	if(e!=mips_Success){
		fprintf(stderr, "mips_cpu_step : failed.\n");
		exit(1);
	}
    
	uint32_t got;
	e = mips_cpu_get_register(cpu, dst, &got);
	int passed;
	if (got == success_value) {
		passed = 1;
	}
	
    stringstream ss;
	ss << name << "failed, with inputs " << valuea << " and  " << valueb << endl;
	
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

	int subtype;
	//1st ADDU test
	
	string name = "addu";
	unsigned srca = 4;
	unsigned srcb = 5;
	unsigned dst = 3;
	int testId = mips_test_begin_test(name.c_str());   
    	subtype = rtestsetup( name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId,name, srca, 40, srcb, 50, dst, 90, cpu);

	
    	//2nd ADDU test	
	
	name = "addu";
	srca = 4;
	srcb = 5;
	dst = 0;
	testId = mips_test_begin_test(name.c_str());   
    subtype = rtestsetup(name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId, name, srca, 40, srcb, 50, dst, 0, cpu);

	
	//1st AND test
	
	name = "and";
	srca = 10;
	srcb = 12;
	dst = 3;
	testId = mips_test_begin_test(name.c_str());   
    subtype = rtestsetup(name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId, name, srca, 0xF0F0F0F0, srcb, 0xFF00FF00, dst, (0xF0F0F0F0 & 0xFF00FF00), cpu);
	
    //1st OR test
	
	name = "or";
	srca = 5;
	srcb = 7;
	dst = 20;
	testId = mips_test_begin_test(name.c_str());   
    subtype = rtestsetup(name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId, name, srca, 0xF0F0F0F0, srcb, 0xFF00FF00, dst, (0xF0F0F0F0 | 0xFF00FF00), cpu);
	
	//1st XOR test
	
	name = "xor";
	srca = 2;
	srcb = 10;
	dst = 8;
	testId = mips_test_begin_test(name.c_str());   
    subtype = rtestsetup(name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId, name, srca, 0xF0F0F0F0, srcb, 0xFF00FF00, dst, (0xF0F0F0F0 ^ 0xFF00FF00), cpu);
	
	//1st SLTU  test
	
	name = "sltu";
	srca = 4;
	srcb = 30;
	dst = 31;
	testId = mips_test_begin_test(name.c_str());   
    subtype = rtestsetup(name, srca, srcb, dst, 0,  mem, cpu);
	subtype1(testId, name, srca, 0xFFFFFFFF, srcb, 0xFFFFFFFE, dst, 0, cpu);
	
	//1st BEQ - Not equal
	
	name = "beq";
	srca = 3;
	srcb = 5;
	uint16_t ioffset = 0xFFFF;
	testId = mips_test_begin_test(name.c_str());   
	subtype = itestsetup(name, srca, srcb, ioffset, mem, cpu);
	subtype2(testId, name, srca, 6, srcb, 5, 4, cpu);
	
	//2nd BEQ - equal with negative offset
	
	name = "beq";
	srca = 3;
	srcb = 5;
	ioffset = 0xFFFF;
	testId = mips_test_begin_test(name.c_str());   
	subtype = itestsetup(name, srca, srcb, ioffset, mem, cpu);
	subtype2(testId, name, srca, 6, srcb, 6, ioffset << 2, cpu);
	
	//3rd BEQ - equal with positive offset
	
	name = "beq";
	srca = 3;
	srcb = 5;
	ioffset = 0x0001;
	testId = mips_test_begin_test(name.c_str());   
	subtype = itestsetup(name, srca, srcb, ioffset, mem, cpu);
	subtype2(testId, name, srca, 6, srcb, 6, ioffset << 2, cpu);
    
	
	mips_test_end_suite();
    
    return 0;
}
