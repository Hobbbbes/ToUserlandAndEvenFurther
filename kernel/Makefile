
OSNAME = ToUserlandAndEvenFurther
GNUEFI = ../bootloader
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -std=c++20 -Iinclude -fno-stack-protector -MMD -MP -fno-exceptions -g 
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib -no-relax
LOOP = $(shell cat loopdevice)
SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/bin/main.efi
EFI_PART_MOUNT_PATH=/mnt/$(OSNAME)/efi
DATA_PART_MOUNT_PATH=/mnt/$(OSNAME)/data
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)          
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

CURRENT_UID := $(shell id -u)
CURRENT_GID := $(shell id -g)

kernel: link

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

img:
	./setup_os_image.sh $(BUILDDIR) $(OSNAME)

setup: img
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)
	@sudo mkdir -p $(EFI_PART_MOUNT_PATH)
	@sudo mkdir -p $(DATA_PART_MOUNT_PATH)
	sudo mkfs -t ext2 $(LOOP)p2
	sudo mkfs -t vfat -F 32 $(LOOP)p1
	sudo mount -t vfat $(LOOP)p1 $(EFI_PART_MOUNT_PATH) -o rw,uid=$(CURRENT_UID),gid=$(CURRENT_GID)
	sudo mount $(LOOP)p2 $(DATA_PART_MOUNT_PATH) -o rw
	mkdir -p $(EFI_PART_MOUNT_PATH)/EFI/BOOT
	cp startup.nsh $(EFI_PART_MOUNT_PATH)
	cp $(BUILDDIR)/zap-light16.psf $(EFI_PART_MOUNT_PATH)/zap-light16.psf
	sudo umount $(LOOP)p1 
	sudo umount $(LOOP)p2
	losetup -d $(LOOP)

add_loop: img
	@echo $(LOOP)
remove_loop: umount
	-losetup -d $(LOOP)
umount:
	-sudo umount $(LOOP)p1 
	-sudo umount $(LOOP)p2
mount: add_loop
	-sudo mount -t vfat $(LOOP)p1 $(EFI_PART_MOUNT_PATH) -o rw,uid=$(CURRENT_UID),gid=$(CURRENT_GID)
	-sudo mount $(LOOP)p2 $(DATA_PART_MOUNT_PATH) -o rw

bootimg: link mount
	-rm $(EFI_PART_MOUNT_PATH)/EFI/BOOT/main.efi
	cp $(BOOTEFI) $(EFI_PART_MOUNT_PATH)/EFI/BOOT/main.efi
	-rm $(EFI_PART_MOUNT_PATH)/kernel.elf
	cp $(BUILDDIR)/kernel.elf $(EFI_PART_MOUNT_PATH)/kernel.elf

clean:
	rm -rf lib/*
	rm -f $(BUILDDIR)/kernel.elf

run_cpu_reset: remove_loop
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -d cpu_reset -net none

run: remove_loop
	qemu-system-x86_64 -d mmu,guest_errors,page -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -net none
debug: remove_loop
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -net none -s&
	gdb -tui -ex "target remote localhost:1234" -ex "symbol-file bin/kernel.elf" -ex "set disassembly-flavor intel" -ex "focus cmd" -ex "set print asm-demangle on" -ex "set print demangle on"
debug-efi: remove_loop
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios /usr/share/ovmf/x64/OVMF.fd -net none -s&
	gdb -tui -ex "target remote localhost:1234" -ex "add-symbol-file ../bootloader/bin/main.so 0xE7B3000 -s .data 0xe7bb000" -ex "set disassembly-flavor intel" -ex "focus cmd" -ex "set print asm-demangle on" -ex "set print demangle on" ../bootloader/bin/main.efi