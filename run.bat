docker run --rm -t --privileged -v .:/root/env os-buildenv make
"C:\Program Files\qemu\qemu-system-i386.exe" -cdrom .\dist\os.iso -serial file:serial.log