# ========================================
# Simple boot32 barebones Makefile
# by Infinity Technology.
# ========================================
BUILD_DIR=./build/

AS=nasm
AFLAGS=-f bin

# Custom commands
IMAGEFS=$(BUILD_DIR)imagefs/imagefs
PRSFS=$(BUILD_DIR)prsfs/prsfs

SRCDIR=.

PROJECT_DIR=$(shell pwd)
PROJECT_NAME=$(shell basename $(PROJECT_DIR))
PROJECT_VERSION=1.0

CDROM_IMG=$(SRCDIR)/boot32-barebones.iso

.PHONY: all run debug disk clean distclean dist
all:
	cd $(SRCDIR)/libs && $(MAKE)
	cd $(SRCDIR)/boot && $(MAKE)
	cd $(SRCDIR)/kernel && $(MAKE)
	cd $(SRCDIR)/imagefs && $(MAKE)
	cd $(SRCDIR)/prsfs && $(MAKE)
	cd $(SRCDIR)/imgwrite && $(MAKE)

run: disk
	qemu-system-i386 -fda $(BUILD_DIR)floppy.img -boot a -soundhw pcspk

debug: disk
	qemu-system-i386 -fda $(BUILD_DIR)floppy.img -boot a -s &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(BUILD_DIR)kernel/kernel.elf"

disk: all
	$(IMAGEFS) c $(BUILD_DIR)floppy.img 200
	$(IMAGEFS) w $(BUILD_DIR)floppy.img $(BUILD_DIR)boot/stage1.bin
	$(PRSFS) $(BUILD_DIR)floppy.img $(BUILD_DIR)boot/stage2.bin $(BUILD_DIR)kernel/kernel.bin

cdrom: disk clean
	cd .. && mkisofs -pad -b $(BUILD_DIR)floppy.img -R -o ./$(PROJECT_NAME)/infinityOS.iso ./$(PROJECT_NAME)

clean:
	rm -f $(TARGETS)
	cd $(SRCDIR)/libs && $(MAKE) clean
	cd $(SRCDIR)/boot && $(MAKE) clean
	cd $(SRCDIR)/kernel && $(MAKE) clean
	cd $(SRCDIR)/imagefs && $(MAKE) clean
	cd $(SRCDIR)/prsfs && $(MAKE) clean
	cd $(SRCDIR)/imgwrite && $(MAKE) clean

distclean: clean
	rm -f floppy.img $(CDROM_IMG) *.bak *.log
	cd $(SRCDIR)/libs && $(MAKE) distclean
	cd $(SRCDIR)/boot && $(MAKE) distclean
	cd $(SRCDIR)/kernel && $(MAKE) distclean
	cd $(SRCDIR)/imagefs && $(MAKE) distclean
	cd $(SRCDIR)/prsfs && $(MAKE) distclean
	cd $(SRCDIR)/imgwrite && $(MAKE) distclean

dist: distclean
	cd .. && tar -cv --exclude=.git $(SRCDIR)/$(PROJECT_NAME) \
| xz -9 > $(PROJECT_NAME)-$(PROJECT_VERSION).tar.xz
