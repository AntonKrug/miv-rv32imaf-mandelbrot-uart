#!/bin/bash


# Use this hardcoded path if any other was not given by enviroment variables
SC_DIR="${SC_DIR:-/opt/microsemi/softconsole/}"


# Find location of this test directory no matter what current directory is
# this might break between shells (tested with bash)
TESTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


# Making sure old OpenOCD is not running
$TESTS_DIR/killOpenOCD.sh


# Run OpenOCD on the background
$SC_DIR/openocd/bin/openocd -f board/microsemi-riscv.cfg &


# Give it enough time for hardware to init
sleep 4 


# Start GDB
echo "OpenOCD should be launched, start the gdb now. Go into the first Debug"
echo "folder I can find (make sure to run clean before this so there are no"
echo "other folders present)."
cd $TESTS_DIR/../Debug*
$SC_DIR/riscv-unknown-elf-gcc/bin/riscv64-unknown-elf-gdb -x ../tests/gdb-test-checksum *.elf


# Store the exit code from GDB
RESULT=$?


# Kill OpenOCD first 
$TESTS_DIR/killOpenOCD.sh


# And then cascade the gdb pass exit code out as a pass and all other
# codes as fails
echo "Got code: $RESULT"
if [ $RESULT == 149 ];
then
    echo "Exiting as PASS"
    exit 0
fi

if [ $RESULT == 150 ];
then
    echo "Exiting as FAILURE as the test failed (the assertions failed)"
else
    echo "Exiting as FAILURE as the process failed (openocd/build/gdb)"
fi
exit -1

