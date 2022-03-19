#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "stdarg.h"
#include "helper.h"
#include "file.h"
#include "memutils.h"
#include "print.h"

int snprintf_cpy(char* buffer, size_t bufferSize, size_t startOffset, const char* instr, unsigned int inlen) {
    const unsigned char *bytes = (const unsigned char *) instr;
	for(unsigned int i = 0; i < inlen; i++) {
		unsigned char byteAt = bytes[i];
		if(byteAt == EOF || (startOffset + i) >= bufferSize - 1) {
			return 0;
		} else {
			buffer[startOffset + i] = instr[i];
		}
	}
	return 1;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list ap;
	int written = 0;
	char leadingNumberChar = 0;
	int leadingNumberCount = 0;
	
	va_start(ap, format);
	while(*format != 0) {
		unsigned maxrem = INT_MAX - written;

		int addToWritten = 0;
		
		// kprintf("%c - %c - leadingNumberCount: %d\n", format[0], format[1], leadingNumberCount);
		// kprintf("%d - %d - %d\n", (leadingNumberCount == 0), (format[0] != '%'), (format[1] == '%'));

		// kprintf("%s\n", format);

		// if(leadingNumberCount == 0 && (format[0] != '%' || format[1] == '%')) {
		if(leadingNumberCount == 0 && (format[0] != '%' || format[1] == '%')) {
			// kprintf("entrou\n");
			if(format[0] == '%') {
				format++;
			}
			unsigned amount = 1;
			while(format[amount] && format[amount] != '%') {
				amount++;
			}

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

			if(maxrem < amount) {
				/* TODO: Set errno to EOVERFLOW. */
				return written;
			}
			if(!snprintf_cpy(buffer, size, written, format, amount)) {
				return written;
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
			if (written >= size - 1) {
				return -1;
			} else {
				buffer[written] = c;
			}
			// kputc(c);
			written++;
		} else if(*format == 's') { /* String */
			format++;
			const char* str = va_arg(ap, const char*);
			unsigned len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
			}
			written += len;
		} else if(*format == 'd' || *format == 'i') { /* Signed Integer */
			format++;
			int c = va_arg(ap, int);
			char str[32] = {0};
			unsigned len;

			itoa_s2(c, 10, str, 32, leadingNumberChar, leadingNumberCount);
			// itoa_s(c, 10, str, 32);
			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
			}
			written += len;
		} else if(*format == 'x' || *format == 'X') { /* Hexadecimal */
			format++;
			int c = va_arg(ap, int);
			char str[32] = {0};
			unsigned len;

			itoa_s2(c, 16, str, 32, leadingNumberChar, leadingNumberCount);
			// itoa_s(c, 16, str, 32);
			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
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
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
			}
			written += len;
		} else if (*format == 'u') {
			format++;
			unsigned int c = va_arg(ap, unsigned int);
			// int **pp = &c;
			char str[32] = {0};
			unsigned len;

			uitoa_s(c, 10, str, 32);
			len = strlen(str);
			// int address = (int) &pp;
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
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
			if(!snprintf_cpy(buffer, size, written, str, len)) {
				return -1;
			}
			written += len;
		} else { /* Normal character */
			format = format_begun_at;
			unsigned len = strlen(format);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, written, format, len)) {
				return -1;
			}
			written += len;
			format += len;
		}
		leadingNumberChar = 0;
		leadingNumberCount = 0;
	}
	va_end(ap);

	if (written < size - 1) {
		buffer[written] = '\0';
		// kprintf("\nzeroWritten: %i\n", written);
	}

	written++;

	return written;
}