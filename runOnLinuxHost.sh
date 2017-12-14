#!/bin/sh

mkdir -p ./Debug
rm Debug/host.elf 2>/dev/null
gcc ./main.c -lm -o Debug/host.elf
chmod a+x Debug/host.elf
./Debug/host.elf
