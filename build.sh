#!/bin/sh
# TODO(Thezo): Eventually(Definitly) shift over to a make build system

set -xe

mkdir -p ./build
pushd ./build

gcc ~/Documents/code/projects/saxa/code/kmain.c -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c

nasm -f elf ~/Documents/code/projects/saxa/code/boot.s
mv ~/Documents/code/projects/saxa/code/boot.o .

ld -T ~/Documents/code/projects/saxa/code/link.ld -melf_i386 boot.o kmain.o -o kernel.elf
cp ./kernel.elf ../iso/boot/

popd

genisoimage -R                      \
    -b boot/grub/stage2_eltorito    \
    -no-emul-boot                   \
    -boot-load-size 4               \
    -A os                           \
    -input-charset utf8             \
    -quiet                          \
    -boot-info-table                \
    -o os.iso                       \
    iso
mv ./os.iso ./build/
