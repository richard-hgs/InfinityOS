BUILD_DIR=../../build/
CURRENT_DIR=libs/$(shell basename $(CURDIR))

LIBC_C_OBJECTS := $(shell find './' -type f -name '*.c')
LIBC_C_OBJECTS := $(patsubst %.c, $(BUILD_DIR)$(CURRENT_DIR)/%.c.o, $(LIBC_C_OBJECTS))

CC=gcc
CFLAGS=-std=c11 -Wall -O -pedantic -g -m32 \
	-ffreestanding -fno-pie -fno-pic -fno-builtin \
	-nostdlib -nostdinc -Wno-multichar -nostartfiles
LDFLAGS=
BACKUPS=$(shell find . -iname "*.bak")
OBJECTS=$(LIBC_C_OBJECTS)
TARGET=$(BUILD_DIR)$(CURRENT_DIR)/libc.a

.PHONY: all test clean distclean
all: $(TARGET)

$(BUILD_DIR)$(CURRENT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<


$(TARGET): $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

test: 
	$(info $$var is [${LIBC_C_OBJECTS}])

clean:
	rm -f $(OBJECTS) $(TARGET) *.elf

distclean: clean
ifneq ($(BACKUPS),)
	rm -f $(BACKUPS)
endif