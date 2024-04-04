docker run --rm -t --privileged -v .:/root/env os-buildenv make && echo "Build succeded" || echo "Build failed"
"C:\Program Files\qemu\qemu-system-i386.exe" -drive id=disk,file=./res/ext_storage.img,if=none, -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -cdrom ./dist/os.iso -serial file:serial.log -m 256M -D log.txt -d cpu_reset,int

pause