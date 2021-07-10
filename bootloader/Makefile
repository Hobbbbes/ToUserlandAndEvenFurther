SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.c)


OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

CC = gcc
LD = ld 

EFI_HEADER = /usr/include/efi
EFI_X64_HEADER = /usr/include/efi/x86_64/


CFLAGS = -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I ${EFI_HEADER} -I ${EFI_X64_HEADER} -Wall -DGNU_EFI_USE_MS_ABI -Dx86_64 -m64
LDFLAGS = -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o -l:libgnuefi.a -l:libefi.a -lefi -lgnuefi

all: setup $(BUILDDIR)/main.efi

$(BUILDDIR)/main.efi: $(BUILDDIR)/main.so
	objcopy -j .text              \
          -j .sdata               \
          -j .data                \
          -j .dynamic             \
          -j .dynsym              \
          -j .rel                 \
          -j .rela                \
          -j .reloc               \
          --target=efi-app-x86_64 \
          $(BUILDDIR)/main.so     \
          $(BUILDDIR)/main.efi

$(BUILDDIR)/main.so: $(OBJ)
	${LD} $(OBJ) ${LDFLAGS} -o $(BUILDDIR)/main.so

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} $< ${CFLAGS} -o $@
 
clean:
	rm -f $(BUILDDIR)/main.so $(BUILDDIR)/main.efi ${OBJ}
setup:
	mkdir -p $(OBJDIR) 
	mkdir -p $(BUILDDIR)
run:
	uefi-run -b /usr/share/edk2-ovmf/x64/OVMF.fd -q /usr/bin/qemu-system-x86_64 $(BUILDDIR)/main.efi
