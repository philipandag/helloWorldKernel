#!/bin/bash

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

if grub-file --is-x86-multiboot myos.bin; then
    echo "multiboot confirmed"
else
    echo "the file is not multiboot"
fi

cp myos.bin isodir/boot
cp grub.cfg isodir/boot/grub

grub-mkrescue -o myos.iso isodir