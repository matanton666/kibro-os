#!/bin/bash

# build and run os. use -c flag to clean before build and -n flag to skip execution

# -c flag to clean before build
if [ "$1" == "-c" ] || [ "$2" == "-c" ] || [ "$3" == "-c" ]; then
    echo "Cleaning..."
    sudo docker run --rm -t --privileged -v .:/root/env os-buildenv make clean
fi

sudo docker run --rm -t --privileged -v .:/root/env os-buildenv make
# if - else clause for the exit status of the last command
if [ $? -eq 0 ]; then
    if [ "$1" == "-n" ] || [ "$2" == "-n" ] || [ "$3" == "-n" ]; then
        echo "Build succeeded, skipping execution"
        exit 0
    fi

    echo "Build succeeded"

    if [ "$1" == "-d" ] || [ "$2" == "-d" ] || [ "$3" == "-d" ]; then
        qemu-system-i386 -S -gdb tcp::1234 -cdrom ./dist/os.iso -serial file:serial.log -m 256M &
        exit 0
    fi
    # qemu-system-i386 -cdrom ./dist/os.iso -serial file:serial.log -m 256M -D log.txt -d cpu_reset,int
    sudo qemu-system-i386 -drive id=disk,file=./res/ext_storage.img,if=none, -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -cdrom ./dist/os.iso -serial file:serial.log -m 256M -D log.txt -d cpu_reset,int 
    exit 0
else
    echo "*Build failed*"
    exit 1
fi
