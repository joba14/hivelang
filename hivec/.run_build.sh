
#!/bin/sh

set -xe

nasm -felf64 ./bin/linux/debug/target.asm
ld -o ./bin/linux/debug/target.out ./bin/linux/debug/target.o
./bin/linux/debug/target.out
