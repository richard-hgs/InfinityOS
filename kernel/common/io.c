/*
 * io.c - Source file for standard output functions.
 *
 * Author: Philip R. Simonson
 * Date  : 07/01/2020
 *
 ****************************************************************
 */

#include <limits.h>
#include <stdbool.h>

// #include "vga.h"
#include "vga2.h"
#include "serial_com.h"
#include "helper.h"
#include "stdarg.h"
#include "file.h"
#include "io.h"
#include "memutils.h"
#include "kstring.h"

/* Print character to console.
 */
static int _kputc(int col, int row, char c)
{
	if (col < 0 || row < 0) {
		vga_get_cursor_position(&col, &row);
	}

	vga_print_char(col, row, -1, -1, c);

	// int offset;
	
	// if(col >= 0 && row >= 0) {
	// 	offset = get_screen_offset(col, row);
	// } else {
	// 	offset = get_cursor_offset();
	// 	row = get_offset_row(offset);
	// 	col = get_offset_col(offset);
	// }
	
	// offset = print_char(col, row, c);
	// row = get_offset_row(offset);
	// col = get_offset_col(offset);
	return c;
}
/* Print character for kernel.
 */
int kputc(char c)
{
	return _kputc(-1, -1, c);
}
/* Print character for kernel.
 */
static int _kputs(const char *s)
{
	int i;
	for(i = 0; i < strlen(s); i++) {
		kputc(s[i]);
	}
	return i;
}
/* Print string to console.
 */
int kputs(const char *s)
{
	return _kputs(s);
}
/* Print character by character to screen with length.
 */
static int _kprint(const char *str, unsigned int len)
{
	const unsigned char *bytes = (const unsigned char *)str;
	for(unsigned int i = 0; i < len; i++) {
		if(kputc(bytes[i]) == EOF) {
			return 0;
		}
	}
	return 1;
}

static int _dsputc(char c) {
	serialcom_write(COM_1_PORT, c);
	return c;
}

static int _dsprint(const char *str, unsigned int len) {
	const unsigned char *bytes = (const unsigned char *)str;
	for(unsigned int i = 0; i < len; i++) {
		if(_dsputc(bytes[i]) == EOF) {
			return 0;
		}
	}
	return 1;
}

// void mitoa_s(int value, unsigned char radix, char *str, int size, char leadChar, int leadCount) {
// 	// uint32_t strOffset = (uint32_t) str;
// 	int written = itoa_s(value, radix, str, size);
// 	if (written < leadCount && written < size - 1) {
// 		// leadCount = 2; written = 1; sizeToCpy = 1;
// 		int leadSize = leadCount - written;
// 		memcpy_r(str + (written - 1) + leadSize, str + (written - 1), written);
// 		memset(str, leadChar, leadSize);
// 		str[written + leadSize] = '\0';
// 	}
// }

/** 
 * Print formatted for kernel.
 * 0=TERMINAL_MODE_SCREEN 1=SERIAL_DEBUG_CONSOLE
 * -------------------------------------------
 * following formats are supported :-
 * format     output type       argument-type
 *   %%        -                   -
 *   %u        unsigned            int
 *   %u*       unsigned            *
 *   %b        binary              int
 *   %lb       binary              long
 *   %hb       binary              char
 *   %d        decimal             int
 *   %lu       unsigned            long
 *   %hu       unsigned            char
 *   %l[di]    decimal             long
 *   %lu[di]   unsigned            long
 *   %h[di]    decimal             char
 *   %hu[di]   unsigned            char
 *   %[xX]     hexadecimal         int
 *   %l[xX]    hexadecimal         long
 *   %h[xX]    hexadecimal         char
 *   %o        octal               int
 *   %lo       octal               long
 *   %ho       octal               char
 *   %c        character           char
 *   %s        character           generic pointer
 * Also supported are:
 * - the '0', '-' and ' ' alignment modifiers
 * - the '+' and ' ' modifiers
 * - the width field for integral types
 * - the precision field for strings
 */
int kprint_type(short type, const char *format, va_list ap) {
	// va_list ap;
	int written = 0;
	char leadingNumberChar = 0;
	int leadingNumberCount = 0;
	
	// va_start(ap, format);
	while(*format != 0) {
		unsigned maxrem = INT_MAX - written;

		int addToWritten = 0;

		if((written == 0 || leadingNumberCount == 0) && ((format[0] != '%') || format[1] == '%')) {
			unsigned amount = 1;
			if (written > 0) {
				if(format[0] == '%') {
					format++;
				}
				while(format[amount] && format[amount] != '%') {
					amount++;
				}
			}

			dskprintf("%c\r\n", format[amount]);
			// If custom format passed fix the format
			if (format[amount] && format[amount] == '%') {
				if (format[amount + 1] && format[amount + 1] == '0') {
					leadingNumberChar = '0';
					addToWritten++;

					char strNum[2] = {'\0'};

					while(format[amount + 1 + addToWritten] && format[amount + 1 + addToWritten] >= '0' && format[amount + 1 + addToWritten] <= '9') {
						strNum[addToWritten - 1] = format[amount + 1 + addToWritten];
						leadingNumberCount = atoi(strNum);
						addToWritten++;

						if (addToWritten > 6) {
							// Block infinite loop
							break;
						}
					}
				}
			}

			// if (format[amount] && format[amount] == '2') {
			// 	leadingNumberCount = 0;
			// 	amount++;
			// }

			// Debug Only
			// if (type == 0) {
			// 	char newFormat[800] = {'\0'};
			// 	int newFormatPos = 0;
			// 	for(int i=0; i<amount; i++) {
			// 		char charAt = format[i];
			// 		if (charAt == '\n') {
			// 			newFormat[newFormatPos] = '\\';
			// 			newFormatPos++;
			// 			newFormat[newFormatPos] = 'n';
			// 		} else {
			// 			newFormat[newFormatPos] = charAt;
			// 		}

			// 		newFormatPos++;
			// 	}

			// 	char str[32] = {0};
			// 	unsigned len;
			// 	int c = leadingNumberCount;

			// 	itoa_s(c, 10, str, 32);
			// 	len = strlen(str);
			// 	_dsprint("leadi: ", 9);
			// 	_dsprint(str, len);
			// 	_dsprint(" | ", 4);
			// 	_dsprint(newFormat, strlen(newFormat));
			// 	_dsprint("\r\n", 2);
			// }
			if(maxrem < amount) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(format, amount)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(format, amount)) {
					return -1;
				}
			}
			format += (amount + addToWritten);
			written += (amount + addToWritten);
			continue;
		}
		
		const char *format_begun_at = format++;
		
		if(*format == 'c') { /* Character */
			format++;
			char c = (char) va_arg(ap, int);
			if(!maxrem) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				kputc(c);
			} else if (type == 1) {
				_dsputc(c);
			}
			written++;
		} else if(*format == 's') { /* String */
			format++;
			const char* str = va_arg(ap, const char*);
			unsigned len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				_kprint(str, len);
			} else if (type == 1) {
				_dsprint(str, len);
			}
			// if(!_kprint(str, len)) {
			// 	return -1;
			// }
			written += len;
		} else if(*format == 'd' || *format == 'i') { /* Signed Integer */
			format++;
			int c = va_arg(ap, int);
			char str[32] = {0};
			unsigned len;

			itoa_s2(c, 10, str, 32, leadingNumberChar, leadingNumberCount);
			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(str, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(str, len)) {
					return -1;
				}
			}
			written += len;
		} else if(*format == 'x' || *format == 'X') { /* Hexadecimal */
			int c = va_arg(ap, int);
			char str[32] = {0};
			unsigned len;

			itoa_s2(c, 16, str, 32, leadingNumberChar, leadingNumberCount);

			if (*format == 'X') {
				kstr_toupper(str, 32);
			}

			format++;

			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(str, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(str, len)) {
					return -1;
				}
			}
			written += len;
		} else if (*format == 'p') { /* Pointer address Hexadecimal */
			format++;
			void* c = va_arg(ap, void*);
			// int **pp = &c;
			char str[32] = {0};
			unsigned len;

			uitoa_s((unsigned long) c, 16, str, 32);
			len = strlen(str);
			// int address = (int) &pp;
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(str, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(str, len)) {
					return -1;
				}
			}
			written += len;
		} else if (*format == 'u') {
			format++;
			uint64_t c = va_arg(ap, uint64_t);
			// int **pp = &c;
			char str[64] = {0};
			unsigned len;

			// uitoa_s(c, 10, str, 32);
			u64toa(c, str, 10);
			len = strlen(str);
			// int address = (int) &pp;
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(str, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(str, len)) {
					return -1;
				}
			}
			written += len;
		}  else if (*format == 'o') {
			format++;
			unsigned int c = va_arg(ap, unsigned int);
			// int **pp = &c;
			char str[32] = {0};
			unsigned len;

			uitoa_s(c, 8, str, 32);
			len = strlen(str);
			// int address = (int) &pp;
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(str, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(str, len)) {
					return -1;
				}
			}
			written += len;
		} else { /* Normal character */
			format = format_begun_at;
			unsigned len = strlen(format);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (type == 0) {
				if(!_kprint(format, len)) {
					return -1;
				}
			} else if (type == 1) {
				if(!_dsprint(format, len)) {
					return -1;
				}
			}
			written += len;
			format += len;
		}
		leadingNumberChar = 0;
		leadingNumberCount = 0;
	}
	// va_end(ap);
	return written;
}

/** 
 * Print formatted for kernel console mode text.
 * -------------------------------------------
 * following formats are supported :-
 * format     output type       argument-type
 *   %%        -                   -
 *   %u        unsigned            int
 *   %u*       unsigned            *
 *   %b        binary              int
 *   %lb       binary              long
 *   %hb       binary              char
 *   %d        decimal             int
 *   %lu       unsigned            long
 *   %hu       unsigned            char
 *   %l[di]    decimal             long
 *   %lu[di]   unsigned            long
 *   %h[di]    decimal             char
 *   %hu[di]   unsigned            char
 *   %[xX]     hexadecimal         int
 *   %l[xX]    hexadecimal         long
 *   %h[xX]    hexadecimal         char
 *   %o        octal               int
 *   %lo       octal               long
 *   %ho       octal               char
 *   %c        character           char
 *   %s        character           generic pointer
 * Also supported are:
 * - the '0', '-' and ' ' alignment modifiers
 * - the '+' and ' ' modifiers
 * - the width field for integral types
 * - the precision field for strings
 */
int kprintf(const char *format, ...) {
	int written = 0;
	va_list ap;
	va_start(ap, format);
	written = kprint_type(0, format, ap);
	va_end(ap);
	return written;
}

/**
 * @brief Print formatted for serial debug console
 * 
 * @param format 	String format
 * @param ... 		Arguments
 * @return int 		Length written
 */
int dskprintf(const char* format, ...) {
	int written = 0;
	va_list ap;
	va_start(ap, format);
	written = kprint_type(1, format, ap);
	va_end(ap);
	return written;
}
