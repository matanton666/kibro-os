#!/bin/bash

# build and run os. use -c flag to clean before build and -n flag to skip execution

# -c flag to clean before build
if [ "$1" == "-c" ]; then
    echo "Cleaning..."
    sudo docker run --rm -t --privileged -v .:/root/env os-buildenv make clean
fi

sudo docker run --rm -t --privileged -v .:/root/env os-buildenv make
# if - else clause for the exit status of the last command
if [ $? -eq 0 ]; then
    if [ "$2" == "-n" ]; then
        echo "Build succeeded, skipping execution"
        exit 0
    fi
    echo "Build succeeded"
    qemu-system-i386 -cdrom ./dist/os.iso -serial file:serial.log -m 256M -D log.txt -d cpu_reset,int,mmu
    exit 0
else
    echo "*Build failed*"
    exit 1
fi
