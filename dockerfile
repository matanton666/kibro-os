FROM techiekeith/gcc-cross-i686-elf

RUN apt-get update -y
RUN apt-get upgrade -y

RUN apt-get install -y nasm xorriso
RUN apt-get install -y grub-pc-bin grub-common
RUN apt-get install -y build-essential mtools
RUN apt-get install -y make nasm gcc g++ gdb
RUN apt-get install -y bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

VOLUME /root/env
WORKDIR /root/env
