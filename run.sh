#!/bin/bash

sudo docker run --rm -t --privileged -v .:/root/env os-buildenv make
# if - else clause for the exit status of the last command
if [ $? -eq 0 ]; then
    echo "Build succeeded"
    qemu-system-i386 -cdrom ./dist/os.iso -serial file:serial.log -m 256M
    exit 0
else
    echo "Build failed"
    exit 1
fi
