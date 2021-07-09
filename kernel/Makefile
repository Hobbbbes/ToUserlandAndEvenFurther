
OSNAME = ToUserlandAndEvenFurther

GNUEFI = ../bootloader
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -std=c++20 -Iinclude -fno-stack-protector -MMD -MP -fno-exceptions -g 
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib -no-relax

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/bin/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)          
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: bootimg

$(OBJDIR)/Interrupts/InterruptServiceRoutines/%.o: $(SRCDIR)/Interrupts/InterruptServiceRoutines/%.cpp
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -mgeneral-regs-only -c $< -o $@


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $< -f elf64 -o $@

link: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

bootimg: link
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT/main.efi
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/zap-light16.psf ::

clean:
	rm -rf lib/*
	rm -f $(BUILDDIR)/CustomOS.img $(BUILDDIR)/kernel.elf

run_cpu_reset:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -d cpu_reset -net none

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -net none
debug:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -net none -s&
	gdb -tui -ex "target remote localhost:1234" -ex "symbol-file bin/kernel.elf" -ex "set disassembly-flavor intel" -ex "focus cmd" -ex "set print asm-demangle on" -ex "set print demangle on"
