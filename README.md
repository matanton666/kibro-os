
# Kibro

an operating system made by matan and yonatan

## build image

`docker build <WORKING_DIR> -t os-buildenv`

## run container

`docker run --rm -it --privileged -v <WORKING_DIR>:/root/env os-buildenv`

## in the container compile

`make`

## in windows run qemu

`& 'C:\Program Files\qemu\qemu-system-i386.exe' -cdrom .\dist\os.iso`

## in wsl/linux run

`qemu-system-x86_64 -cdrom ./dist/iso/os.iso`

## or can also run with the bat script

`.\run.bat`
