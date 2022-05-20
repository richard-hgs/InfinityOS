/*
 * shell.c - Source file for a simplistic shell.
 *
 * Author: Infinity Technology
 * Date  : 04/10/2021
 *
 *********************************************************************
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// #include "vga.h"
#include "vga2.h"
#include "helper.h"
#include "io.h"
#include "shell.h"
#include "timer.h"
#include "system.h"
#include "regs.h"
#include "acpi.h"
#include "memutils.h"
#include "kstring.h"
#include "ports.h"
#include "keyboard.h"
#include "bitwise.h"
#include "print.h"
#include "checksum.h"
#include "kheap.h"
#include "kmalloc.h"
#include "serial_com.h"

/* Login active external variable */
extern bool login_active;
extern bool regs_update;

/* Command structure */
typedef struct command {
	char *cmd;
	char *help;
	void (*func)(void);
} command_t;

/* Some macros for conversion */
#define TO_STR(c) #c
#define TO_CMD(c) cmd_ ##c

/* Some macros for building commands */
#define BEG_CMD const command_t commands[] = {
#define ADD_CMD(c,h) 	{ #c, h, &cmd_ ##c }
#define END_CMD };
#define CNT_CMD const int cmd_total = \
	sizeof(commands)/sizeof(command_t);

/* Macro to define command function */
#define DEF_FNC(c) void cmd_ ##c(void)

/* Prototypes for commands */
DEF_FNC(clear);				// 0
DEF_FNC(regs);				// 1
DEF_FNC(acpidev);			// 2
DEF_FNC(vgatest);			// 3
DEF_FNC(serial_com_test);	// 4
DEF_FNC(trowexception);		// 5
DEF_FNC(kprintf_usage);		// 6
DEF_FNC(memusage);			// 7
DEF_FNC(memtest);			// 8
DEF_FNC(logout);			// 9
DEF_FNC(version);			// 10
DEF_FNC(help);				// 11
DEF_FNC(exit);				// 12

/* ----------------------------- Commands -------------------------- */

BEG_CMD
ADD_CMD(clear, "Clear the VGA screen buffer."),
ADD_CMD(regs, "Display register values."),
ADD_CMD(acpidev, "Acpi development"),
ADD_CMD(vgatest, "Vga Test"),
ADD_CMD(serial_com_test, "Test serial communication"),
ADD_CMD(trowexception, "Trow a exception"),
ADD_CMD(kprintf_usage, "Show the kprintf usage examples"),
ADD_CMD(memusage, "Show the memory usage"),
ADD_CMD(memtest, "Test memory allocation"),
ADD_CMD(logout, "Log out of the operating system."),
ADD_CMD(version, "Displays the version information."),
ADD_CMD(help, "Display this help text."),
ADD_CMD(exit, "Halt execution of CPU.")
END_CMD
CNT_CMD

/* ----------------------------- Functions ------------------------- */

/* Clear command, erase VGA screen buffer.
 */
DEF_FNC(clear)
{
	vga_clear_screen();
}
/* Regs command, display register values.
 */
DEF_FNC(regs)
{
	regs_t *regs;

	// Update register values and get regsters.
	regs = get_current_regs(true);

	// Display results.
	kprintf("List of registers below...\n");
	kprintf("==========================\n");
	kprintf("EAX: 0x%x\nEBX: 0x%x\nECX: 0x%x\nEDX: 0x%x\nESI: 0x%x\nEDI: 0x%x\n",
		regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
}

DEF_FNC(acpidev)
{
	acpi_locate_rsdt();

	// size_t bufferSize = 9;
    // unsigned char buffer[9] = {0x08, 0x01, 0x4E, 0x49, 0x58, 0x42, 0x45, 0x45, 0x3B};
    // size_t ignoreBytesSize = 1;
    // size_t ignoreBytesBuff[1] = {8};
	// checksum_8bit(buffer, bufferSize, ignoreBytesBuff, ignoreBytesSize);

	// count_memory();

	// kbd_set_scancode_set(0x03);
	// kprintf("keyboard scan code: %i", kbd_get_scancode_set());
	// int c = 10;
	// char str[32] = {};

	// prntnum(c, 16, ' ', str);

	// kprintf("acpidev: %s\n", str);

	// char str2[32] = {0};

	// itoa_s(c, 16, str2, 32);

	// kprintf("acpidev2: %s\n", str2);
}

DEF_FNC(vgatest) {
	vga_test();
}

DEF_FNC(serial_com_test) {
	dskprintf("testando\r\n123");
	// serialcom_test();
}

DEF_FNC(trowexception) 
{
	// Division by zero
	__asm__ ("div %0" :: "r"(0));
}

DEF_FNC(kprintf_usage) {
	int myInt = 10;
	int* myIntP = &myInt;

	const char kprintfMsg[] = "kprintf usages:" 
	"\n - (    string    ) %%s   -> %s"
	"\n - (     char     ) %%c   -> %c"
	"\n - (     int      ) %%i   -> %i"
	"\n - (     int      ) %%d   -> %d"
	"\n - (     int      ) %%06d -> %06d"
	"\n - ( unsigned int ) %%u   -> %u"
	"\n - ( unsigned oct ) %%o   -> %o"
	"\n - (int to hex_str) %%02x -> %02x"
	"\n - (int to hex_str) %%02X -> %02X"
	"\n - (ptr to hex_str) %%p   -> %p" 
	"\n";

	int intOverbuff = 2147483647;
	unsigned int uintOverbuff = 4294967295;

	kprintf(kprintfMsg, 
		"uma string",
		75,
		intOverbuff,
		10,
		123,
		(uint64_t) uintOverbuff,
		10,
		10,
		10,
		myIntP
	);

	// char snPrintText[100];
	// snprintf(snPrintText, 100, "%dmeu texto: %02d %02d %06d\0", 2, 2, 5, 20);

 	// kprintf("%s\n", snPrintText);

	/* POINTER EXAMPLE */
	// ptrdiff_t address = ((void*) myIntP) - NULL;

	// void* myPtr = (void*) 0x8FF24;
	// int* myPtrInt = (int*) myPtr;
	// int myPtrIntT = (int) *myPtrInt;

	// kprintf("%x\n", address);
	// kprintf("%p\n", myIntP);
	// kprintf("%i\n", myPtrIntT);
}

DEF_FNC(memusage) {
	kprintf("HEAP   - blockCount: %i - alloc: %i - used: %i - malloc: %i - mused: %i\n", kheapMem.blockCount, kheapMem.heapSpaceAllocated, kheapMem.heapSpaceUsed, kheapMem.heapMemAllocated, kheapMem.heapMemUsed);
	kprintf("KERNEL - mused: %i bytes - %i Kb\n", kheap_get_kernel_static_mem_usage(), kheap_get_kernel_static_mem_usage() / 1024);
	kprintf("MEMORY SNAPSHOT:\n");

	if (kheapMem.blockCount > 0) {
		HeapMemBlock_t* currentBlock = 0;
		size_t currentBlockAddress = 0;
		size_t lastBlockAddress = (size_t) kheap_getMemBlockDataAddress(kheapMem.lastBlock);
		while(currentBlock == 0 || currentBlockAddress <= lastBlockAddress) {
			if (currentBlock != 0) {
				size_t size = (size_t) kheap_getMemBlockDataSize(currentBlock);
				// First bit meory in use
				kprintf("  OFFSET: %i - MAX_OFFSET: %i - SIZE: %i - FLAGS: (%s)\n", currentBlockAddress, lastBlockAddress, size, uint8_t_getbit(currentBlock->flags, 0) == 1 ? "IN_USE" : "FREE");
				hexDump(0, (void*) currentBlockAddress, (int) size, 16);
			}

			if (currentBlock == 0) {
				currentBlock = kheapMem.firstBlock;
			} else {
				currentBlock = currentBlock->nextBlock;
			}
			currentBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentBlock);

			// kprintf("CURRENT_BLOCK: %i - LAST_BLOCK: %i\n", currentBlockAddress, lastBlockAddress);
		}
	}
	kprintf("MEMORY SNAPSHOT END\n\n");
}

DEF_FNC(memtest) {
	int memusageOffset = 7;
	commands[memusageOffset].func();
	char* myStr1 = kmalloc(8);
	memcpy(myStr1, "abcdefgh", 8);
	commands[memusageOffset].func();
	// kfree(myStr1);
	// commands[memusageOffset].func();
	char* myStr2 = kmalloc(8);
	memcpy(myStr2, "bcdefghi", 8);
	commands[memusageOffset].func();
	kfree(myStr1);
	kfree(myStr2);
	commands[memusageOffset].func();
	char* myStr3 = kmalloc(16);
	memcpy(myStr3, "0123456789ABCDEF", 16);
	commands[memusageOffset].func();
	kfree(myStr3);
	commands[memusageOffset].func();
	char* myStr4 = kmalloc(8);
	memcpy(myStr4, "GHIJKLMN", 8);
	commands[memusageOffset].func();
	kfree(myStr4);
	commands[memusageOffset].func();
}

/* Logout command, quit the shell and return to login screen.
 */
DEF_FNC(logout)
{
	kprintf("Logging out...\n");
	sleep(1);
	kprintf("OK.\n");
	login_active = true;
	sleep(3);
	sound(500);
	delay(8);
	sound(0);
	vga_clear_screen();
	kprintf("LOGIN? ");
}
/* Version command, display version information.
 */
DEF_FNC(version)
{
	const char msg[] = "InfinityOS 1.0.0\n";
	kprintf(msg);
	// kprintf("Barebones Operating System ");
	// for(int i = 0; i < strlen(msg); i++) {
	// 	kputc(msg[i]);
	// 	sound(1000);
	// 	delay(10);
	// 	sound(0);
	// 	delay(8);
	// }
	// kprintf("\nDate created: June 25, 2020.\n");
	// kprintf("Written by Infinity Technology.\n\n");
	// kprintf("Others helped me to fix the code I made, just \n");
	// kprintf("a few fixes here and there. But, for the most \n");
	// kprintf("part it is all my code.\n");
}
/* Help command, display help.
 */
DEF_FNC(help)
{
	kprintf("*** Available Commands ***\n");
	for(int i = 0; i < cmd_total; i++)
		kprintf("%s - %s\n", commands[i].cmd, commands[i].help);
}
/* Exit command, halt CPU.
 */
DEF_FNC(exit)
{
	kprintf("Halting CPU...\n");
	disable();
	halt();
}
/* Process user input.
 */
void process_command(char *input)
{
	for(int i = 0; i < cmd_total; i++) {
		if(!strcmp(commands[i].cmd, input)) {
			commands[i].func();
			return;
		}
	}
	kprintf("Invalid command entered.\n");
}
