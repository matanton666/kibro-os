.PHONY: clean all

os_bin_file := dist/boot/os.bin
os_iso_file := dist/os.iso
linker_file := src/linker.ld
headers_folder := src/headers

asm_source_files := $(wildcard src/*.s)
cpp_source_files := $(wildcard src/*.cpp)
cpp_header_files := $(wildcard $(headers_folder)/*.h)

asm_object_files := $(patsubst src/%.s, build/%.o, $(asm_source_files))
cpp_object_files := $(patsubst src/%.cpp, build/%.o, $(cpp_source_files))

# $< is dependency, $@ is target

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
$(asm_object_files): build/%.o: src/%.s
	@mkdir -p build
	i686-elf-as $< -o $@
	@echo "compiled $<"

$(cpp_object_files): build/%.o: src/%.cpp $(cpp_header_files)
	@mkdir -p build
	i686-elf-g++ -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I $(headers_folder) -c  -o $@ $<
	@echo "compiled $<"



# other functions
all: os_iso_file
	@echo "all done"

clean:
	rm -rf build
	rm $(os_bin_file)
	rm $(os_iso_file)
	@echo "cleaned"

