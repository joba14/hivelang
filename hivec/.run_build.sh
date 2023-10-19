
#!/bin/sh

set -xe

./bin/linux/debug/hivec -o ./bin/linux/debug/target.asm ./bin/linux/debug/target.hlang
nasm -felf64 ./bin/linux/debug/target.asm
ld -o ./bin/linux/debug/target.out ./bin/linux/debug/target.o
./bin/linux/debug/target.out
