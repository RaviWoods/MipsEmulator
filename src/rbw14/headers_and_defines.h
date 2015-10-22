/*! \file mips.h
    Main include file for all the simulation and test related files.
*/
#ifndef headers_and_defines_h
#define headers_and_defines_h

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

#define DEBUG_LEVEL 1

class inst_codes_class {
  private:
    static map<string,uint_32> r_functions;
};
#endif