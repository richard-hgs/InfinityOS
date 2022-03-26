/*
 * kernel.c - Source file for kernel, main operation source code.
 *
 * Author: Philip R. Simonson
 * Date  : 06/27/2020
 *
 ***********************************************************************
 */

#include <stdbool.h>
#include <limits.h>

#include "kernel.h"
#include "isr.h"
#include "gdt.h"
// #include "vga.h"
#include "vga2.h"
#include "helper.h"
#include "io.h"
#include "shell.h"
#include "keyboard.h"
#include "timer.h"
#include "system.h"
#include "memutils.h"
#include "kmalloc.h"
#include "serial_com.h"

#define isascii(c) ((unsigned)(c) <= 0x7F)
#define MAXBUF 100
#define DEBUG_ON true

#define DEBUG_COMMANDS_LENGTH 2
int currentDebugCommand = 0;
char DEBUG_COMMANDS_TO_RUN[DEBUG_COMMANDS_LENGTH][MAXBUF] = {"root\0", "kprintf_usage\0"};

bool kbd_istyping;
bool login_active;
bool regs_update;

/* Get key from keyboard. */
int getch(void)
{
	KEYCODE key = KEY_UNKNOWN;
	
	while(key == KEY_UNKNOWN) {
		key = kbd_get_last_key();
	}
	kbd_discard_last_key();
	return key;
}

/* Get command from user and put to buffer. */
void get_command(char *buf, int size)
{
		KEYCODE key;
		bool buf_char;
		int i = 0;

		while(i < size) {
			buf_char = true;
			key = getch();

			if(key == KEY_RETURN) {
				kputc('\n');
				kbd_istyping = false;
				break;
			}

			vga_on_key_pressed(key);

			if(key == KEY_BACKSPACE) {
				if(i > 0) {
					buf[i] = '\0';
					kputc('\b');
					--i;
				}
				continue;
			}

			// Update if keyboard is typing or not.
			kbd_istyping = buf_char;

			if(buf_char) {
				if(isascii(key)) {
					char c = kbd_key_to_ascii(key);
					buf[i++] = c;
					kputc(c);
				}
			}

			delay(3);
		}
		buf[i] = '\0';
}

/* Entry point for kernel.
 */
void kernel_main() {
	// vga_video_init();
	// kmalloc_test();

	// const unsigned short snd[] = {500, 1000, 3000, 1500, 800};
	// const int tsnd = sizeof(snd)/sizeof(snd[0]);
	char key_buffer[MAXBUF];

	// Initialize the variables.
	kbd_istyping = false;
	login_active = true;

	// Initialize the debug serial com port
	serialcom_init_port(COM_1_PORT);

	// Initialize the terminal and install ISRs and IRQs.
	vga_video_init();

	// while(true) {

	// }

	gdt_install();
	isr_install();
	irq_install();

	// Code to calculate cpu speed
	// uint64_t startTime = get_timer_ticks();
	// volatile int count = 0;
	// // 3 Cycles per loop 160000000 / 6038ms - 533mhz ram - 1.6ghz cpu - 80 Milhões de ciclos por segundo
	// while(true) {
	// 	if (count >= 160000000) {
	// 		break;
	// 	}
	// 	count++;
	// }
	// uint64_t endTime = get_timer_ticks();
	// kprintf("processorCount: %i | %u - %u", count, startTime, endTime);
	// while(true) {}

	// // Display loading message and play music.
	// kprintf("Loading system! Please wait");
	// for(int i = 0; i < tsnd; i++) {
	// 	sound(snd[i]);
	// 	delay(5);
	// 	sound(0);
	// 	sleep(1);
	// 	kputc('.');
	// }
	// sound(0);
	// clear_screen();

	// Display welcome message to user and prompt.
	kprintf(WELCOME_MESSAGE);

	// Discard any key sent before the kernel
	kbd_discard_last_key();

	// kprintf("Timer Ticks: %u - MAX: %u\n", get_timer_ticks(), UINT64_MAX);

	while(true) {
		if (DEBUG_ON && DEBUG_COMMANDS_LENGTH > 0 && currentDebugCommand < DEBUG_COMMANDS_LENGTH) {
			kbd_istyping = true;
			char* strDebugCmd = DEBUG_COMMANDS_TO_RUN[currentDebugCommand];
			currentDebugCommand++;

			kprintf("%s\n", strDebugCmd);
			memcpy(key_buffer, strDebugCmd, strlen(strDebugCmd) + 1);
		} else {
			get_command(key_buffer, sizeof(key_buffer));
		}

		// Handle login
		if(login_active) {
			if(!strcmp("root", key_buffer)) {
				login_active = false;
				vga_clear_screen();
				kprintf("Login successful!\n");
				// sleep(2);
				kprintf("Please type 'help' for a list of commands.\n> ");
			} else {
				login_active = true;
				kprintf("Login failed '%s' \nLOGIN? ", key_buffer);
			}
		} else {
			process_command(key_buffer);
			if(!login_active) {
				kprintf("> "); 
			}
		}
	}
}
