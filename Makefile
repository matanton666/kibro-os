.PHONY: clean all

asm_source_files := $(wildcard src/*.asm)
cpp_source_files := $(wildcard src/*.cpp)

asm_object_files := $(patsubst src/%.asm, build/%.o, $(asm_source_files))
cpp_object_files := $(patsubst src/%.cpp, build/%.o, $(cpp_source_files))

os_bin_file := dist/boot/os.bin
os_iso_file := dist/os.iso
linker_file := src/linker.ld


# add grub
os_iso_file: os_bin_file
	rm -f $(os_iso_file)
	grub-mkrescue -o $(os_iso_file) dist/
	@echo "compiled final iso $<"

# link all object files
os_bin_file: $(asm_object_files) $(cpp_object_files) $(linker_file)
	@mkdir -p dist
	i686-elf-g++ -T $(linker_file) -o $(os_bin_file) -ffreestanding -O2 -nostdlib $(asm_object_files) $(cpp_object_files) -lgcc
	@echo "linked all object files"


# compile when src files change
$(asm_object_files): $(asm_source_files)
	@mkdir -p build
	i686-elf-as $< -o $@
	@echo "compiled $<"

$(cpp_object_files): $(cpp_source_files)
	@mkdir -p build
	i686-elf-g++ -c $< -o $@ -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
	@echo "compiled $<"





# other functions
all: os_iso_file
	@echo "all done"

clean:
	rm -rf build
	rm $(os_bin_file)
	rm $(os_iso_file)
	@echo "cleaned"

