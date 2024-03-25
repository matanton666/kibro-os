.PHONY: clean all

os_bin_file := dist/boot/os.bin
os_iso_file := dist/os.iso
os_ext_storage_file := res/ext_storage.img
linker_file := src/kernel/linker.ld
headers_folder := src/headers

asm_source_files := $(wildcard src/kernel/**/*.s) $(wildcard src/kernel/*.s)
cpp_source_files := $(wildcard src/kernel/**/*.cpp) $(wildcard src/kernel/*.cpp)
cpp_header_files := $(wildcard $(headers_folder)/*.h)
psf_object_files := $(wildcard res/*.psf)

asm_object_files := $(patsubst src/kernel/%.s, build/kernel/%.o, $(asm_source_files))
cpp_object_files := $(patsubst src/kernel/%.cpp, build/kernel/%.o, $(cpp_source_files))
psf_object_files := $(patsubst res/%.psf, build/%.o, $(psf_object_files))

# $< is dependency, $@ is target

# add grub
$(os_iso_file): $(os_bin_file) $(os_ext_storage_file)
	rm -f $(os_iso_file)
	grub-mkrescue -o $(os_iso_file) dist/
	@echo "compiled final iso $<"

# link all object files
$(os_bin_file): $(asm_object_files) $(cpp_object_files) $(psf_object_files) $(linker_file) build/kernel/interrupts.o
	@mkdir -p dist
	i686-elf-g++ -T $(linker_file) -o $(os_bin_file) -ffreestanding -O0 -nostdlib $(psf_object_files) $(asm_object_files) $(cpp_object_files) build/kernel/interrupts.o -lgcc
	@echo "linked all object files"

# compile interrupts with separate flags
build/kernel/interrupts.o: src/interrupts/interrupts.cpp $(headers_folder)/interrupts.h
	@mkdir -p build/kernel
	i686-elf-g++ -mno-red-zone -mgeneral-regs-only -ffreestanding -I $(headers_folder) -g -O0 -c -o $@ $<
	@echo "compiled $<"

# compile when src files change
$(asm_object_files): build/kernel/%.o: src/kernel/%.s
	@mkdir -p $(dir $@)
	i686-elf-as -g -O0 $< -o $@
	@echo "compiled $<"

$(cpp_object_files): build/kernel/%.o: src/kernel/%.cpp $(cpp_header_files)
	@mkdir -p $(dir $@)
	i686-elf-g++ -g -fshort-wchar -ffreestanding -O0 -Wall -Wextra -fno-exceptions -fno-rtti -I $(headers_folder) -c -o $@ $<
	@echo "compiled $<"

$(psf_object_files): build/%.o: res/%.psf
	objcopy -O elf32-i386 -B i386 -I binary $< $@
	@echo "compiled $<"

# create external storage image
$(os_ext_storage_file):
	@mkdir -p dist
	dd 'if=/dev/zero' 'of=$@' 'bs=1M' 'count=100'
	@echo "created external storage image"

# other functions
all: $(os_iso_file)
	@echo "all done"

clean:
	rm -rf build
	rm $(os_bin_file)
	rm $(os_iso_file)
	rm $(os_ext_storage_file)
	@echo "cleaned"
