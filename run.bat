docker run --rm -t --privileged -v .:/root/env os-buildenv make && echo "Build succeded" || echo "Build failed"
"C:\Program Files\qemu\qemu-system-i386.exe" -cdrom .\dist\os.iso -serial file:serial.log -m 256M