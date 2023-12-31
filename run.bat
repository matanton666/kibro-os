REM build and run os. use -c flag to clean before build and -n flag to skip execution

@echo off

REM -c flag to clean before build
if "%1"=="-c" (
    echo Cleaning...
    docker run --rm -t --privileged -v .:/root/env os-buildenv make clean
)

docker run --rm -t --privileged -v .:/root/env os-buildenv make
REM if - else clause for the exit status of the last command
if %errorlevel% equ 0 (
    if "%2"=="-n" (
        echo Build succeeded, skipping execution
        exit /b 0
    )
    echo Build succeeded
    REM Adjust the path to qemu-system-i386.exe based on your QEMU installation
    "C:\Program Files\qemu\qemu-system-i386.exe" -cdrom .\dist\os.iso -serial file:serial.log -m 256M -D log.txt -d cpu_reset
    exit /b 0
) else (
    echo *Build failed*
    exit /b 1
)