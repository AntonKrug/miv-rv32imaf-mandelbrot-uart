/*******************************************************************************
 * (c) Copyright 2018 Microsemi SoC Products Group. All rights reserved.
 * v1.0 2017/12/13 anton.krug@microsemi.com
 ******************************************************************************/

#include <stdio.h>
#include <float.h>
#include "test-utils.h"

unsigned int actualChecksum = 0;


// this variable can be set by different defines when using different compile
// configurations and then it can detected with GDB on runtime and distinguish
// what results the tests should use as the GDB test needs to have different
// expected results for soft/hard floats as they slightly differ in the implementation.
unsigned int getConfigurationState() {
  unsigned int ret = 0;

  ret |= CONFIGURATION_RISCV;
  ret |= CONFIGURATION_OPTIMALIZATION_0;

#ifdef HARD_FLOAT_DESIGN
  ret |= CONFIGURATION_HARDFLOAT;
#endif

  return ret;
}


void testAddToChecksumInt(unsigned int checksum) {
  actualChecksum += checksum;
}


void testAddToChecksumFloat(float value) {
  actualChecksum += *(unsigned int*)&value;
}


void testVerifyBreak() {
  // when testing with gdb, place breakpoint here
#ifdef GDB_TESTING
  printf("Test point reached \n");
  while (1);
#endif
}


void testVerify() {
#ifdef GDB_TESTING
  unsigned int current_configuration = getConfigurationState();
  testVerifyBreak();
#endif
}

