#include "headers_and_defines.h"

void rtestsetup(string name, unsigned long opcode, unsigned long srca, unsigned long srcb, unsigned long dst, unsigned long shift, mips_mem_h mem) {
	 
	//int subtype;
    unsigned long function;
	if (name == "addu") {
		function = 0x21;
		//subtype = 1;
	} else if (name == "and") {
		function = 0x24;
		//subtype = 1;
	}
    // 1 - Setup an instruction in ram
    // addu r3, r4, r5
    uint32_t instr =
        (opcode << 26) // opcode = 0
        |
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
    
    mips_error e = mips_mem_write(
        mem,	        //!< Handle to target memory
        0,	            //!< Byte address to start transaction at
        4,	            //!< Number of bytes to transfer
        buffer	        //!< Receives the target bytes
    );
    if(e!=mips_Success){
        fprintf(stderr, "mips_mem_write : failed.\n");
        exit(1);
    }
	//return subtype;
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

	//1st ADDU test
	
	
    rtestsetup("addu", 0, 4, 5, 3, 0,  mem);

    e = mips_cpu_set_register(cpu, 4, 40);
    e = mips_cpu_set_register(cpu, 5, 50);
    
    e = mips_cpu_step(cpu);
    if(e!=mips_Success){
        fprintf(stderr, "mips_cpu_step : failed.\n");
        exit(1);
    }
    
    uint32_t got;
    e=mips_cpu_get_register(cpu, 3, &got);
    
   
    int passed = got == 40+50;
    
    mips_test_end_test(testId, passed, "40 + 50 != 90");
	
    //2nd ADDU test	
    testId = mips_test_begin_test("addu");   
    rtestsetup("addu", 0, 4, 5, 0, 0, mem);
    
    e=mips_cpu_set_register(cpu, 4, 40);
    e=mips_cpu_set_register(cpu, 5, 50);
    
    e=mips_cpu_step(cpu);
    if(e!=mips_Success){
        fprintf(stderr, "mips_cpu_step : failed.\n");
        exit(1);
    }
    
    e = mips_cpu_get_register(cpu, 0, &got);
    
   
    passed = got == 0;
    
    mips_test_end_test(testId, passed, "r0 != 0");

	
	//1st AND test
	
	testId = mips_test_begin_test("and"); 
	
    rtestsetup("and", 0, 10, 12, 3, 0,  mem);

    e = mips_cpu_set_register(cpu, 10, 0xF0F0F0F0);
    e = mips_cpu_set_register(cpu, 12, 0xFF00FF00);
    
    e = mips_cpu_step(cpu);
    if(e!=mips_Success){
        fprintf(stderr, "mips_cpu_step : failed.\n");
        exit(1);
    }
    
    e=mips_cpu_get_register(cpu, 3, &got);
    
   
    passed = got == (0xF0F0F0F0 & 0xFF00FF00);
    
    mips_test_end_test(testId, passed, "0xF0F0F0F0 & 0xFF00FF00 != 0xF000F000");	
    
    
	
	mips_test_end_suite();
    
    return 0;
}
