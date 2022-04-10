#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "stdarg.h"
#include "helper.h"
#include "file.h"
#include "memutils.h"
#include "print.h"

static bool isCustomFormat(int written, const char *format) {
	return (written == 0 && format[1] >= '0' && format[1] <= '9');
}

int snprintf_cpy(char* buffer, size_t bufferSize, size_t startOffset, const char* instr, unsigned int inlen) {
	// kprintf("inStr: %s - inLen: %d - startOffset: %d\n", instr, inlen, startOffset);
    const unsigned char *bytes = (const unsigned char *) instr;
	for(unsigned int i = 0; i < inlen; i++) {
		unsigned char byteAt = bytes[i];
		// kprintf("offset: %04d - value: %04d - %c\n", startOffset + i, (int) byteAt, byteAt);
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
	int writtenReal = 0;
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
		if((written == 0 || leadingNumberCount == 0) && ((format[0] != '%') || format[1] == '%' || isCustomFormat(written, format))) {
			unsigned amount = 1;
			if (written > 0) {
				if(format[0] == '%') {
					format++;
				}
				while(format[amount] && format[amount] != '%') {
					amount++;
				}
			}

			// If custom format in first position matches the first position if don't 
			// proceed with the default increment as next format
			if (isCustomFormat(written, format)) {
				amount = 0;
			}

			// If custom format passed fix the format
			if (format[amount] && format[amount] == '%') {
				int addToFormatOffset = 1;
				if (format[amount + addToFormatOffset] && format[amount + addToFormatOffset] == '0') {
					leadingNumberChar = '0';
					addToWritten++;

					char strNum[2] = {'\0'};

					while(format[amount + addToFormatOffset + addToWritten] && format[amount + addToFormatOffset + addToWritten] >= '0' && format[amount + addToFormatOffset + addToWritten] <= '9') {
						strNum[addToWritten - 1] = format[amount + addToFormatOffset + addToWritten];
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
			if(!snprintf_cpy(buffer, size, writtenReal, format, amount)) {
				return written;
			}
			format += (amount + addToWritten);
			written += (amount + addToWritten);
			writtenReal += (amount);
			continue;
		}
		
		const char *format_begun_at = format++;

		// kprintf("format: %c - leadingNumberCount: %i - leadingNumberChar: %c - written: %d - writtenReal: %d\n", *format, leadingNumberCount, leadingNumberChar, written, writtenReal);
		
		if(*format == 'c') { /* Character */
			format++;
			char c = (char) va_arg(ap, int);
			if(!maxrem) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if (writtenReal >= size - 1) {
				return -1;
			} else {
				buffer[writtenReal] = c;
			}
			// kputc(c);
			written++;
			writtenReal++;
		} else if(*format == 's') { /* String */
			format++;
			const char* str = va_arg(ap, const char*);
			unsigned len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
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

			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
		} else if(*format == 'x' || *format == 'X') { /* Hexadecimal */
			format++;
			int c = va_arg(ap, int);
			char str[32] = {0};
			unsigned len;

			itoa_s2(c, 16, str, 32, leadingNumberChar, leadingNumberCount);

			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
		} else if (*format == 'p') { /* Pointer address Hexadecimal */
			format++;
			void* c = va_arg(ap, void*);
			char str[32] = {0};
			unsigned len;

			uitoa_s((unsigned long) c, 16, str, 32);
			len = strlen(str);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
		} else if (*format == 'u') {
			format++;
			unsigned int c = va_arg(ap, unsigned int);
			char str[32] = {0};
			unsigned len;

			uitoa_s(c, 10, str, 32);
			len = strlen(str);

			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
		}  else if (*format == 'o') {
			format++;
			unsigned int c = va_arg(ap, unsigned int);
			char str[32] = {0};
			unsigned len;

			uitoa_s(c, 8, str, 32);
			len = strlen(str);

			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, str, len)) {
				return -1;
			}
			written += len;
			writtenReal += len;
		} else { /* Normal character */
			format = format_begun_at;
			unsigned len = strlen(format);
			if(maxrem < len) {
				/* TODO: Set errno to EOVERFLOW. */
				return -1;
			}
			if(!snprintf_cpy(buffer, size, writtenReal, format, len)) {
				return -1;
			}
			written += len;
			format += len;
			writtenReal += len;
		}
		leadingNumberChar = 0;
		leadingNumberCount = 0;
	}
	va_end(ap);

	if (writtenReal < size - 1) {
		buffer[writtenReal] = '\0';
		// kprintf("\nzeroWritten: %i\n", written);
	}

	written++;
	writtenReal++;

	return writtenReal;
}