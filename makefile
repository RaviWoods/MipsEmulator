# Your login. For example, mine is dt10. Yours
# won't be eie2ugs...
LOGIN ?= rbw14

CPPFLAGS += -W -Wall -g
CPPFLAGS += -I include

CFLAGS	 += -std=c99
CXXFLAGS += -std=c++11

# Force the inclusion of C++ standard libraries
LDLIBS += -lstdc++

DEFAULT_OBJECTS = \
    src/shared/mips_test_framework.o \
    src/shared/mips_mem_ram.o 

USER_CPU_SRCS = \
    $(wildcard src/$(LOGIN)/mips_cpu.c) \
    $(wildcard src/$(LOGIN)/mips_cpu.cpp) \
    $(wildcard src/$(LOGIN)/mips_cpu_*.c) \
    $(wildcard src/$(LOGIN)/mips_cpu_*.cpp)
    
USER_CPU_OBJECTS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(USER_CPU_SRCS)))

src/$(LOGIN)/test_mips : $(DEFAULT_OBJECTS) $(USER_CPU_OBJECTS)

fragments/run_fibonacci : $(DEFAULT_OBJECTS) $(USER_CPU_OBJECTS)
    
fragments/run_addu : $(DEFAULT_OBJECTS) $(USER_CPU_OBJECTS)

