/*
 * helper.h - Miscellaneous functions for integers and strings.
 *
 * Author: Philip R. Simonson
 * Date  : 07/04/2020
 *
 ********************************************************************
 */
#include <stddef.h>
#include <stdint.h>
#include "helper.h"
#include "memutils.h"
#include "timer.h"

/* ------------------------ Used internally ---------------------- */

char _itoa[32];
const char _hexdigits[] = {
	'0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', 'A', 'B', 'C', 'D',
	'E', 'F'
};

/* ----------------------- Helper Functions ---------------------- */

/* Get length of given string.
 */
int strlen(const char *s)
{
	int i = 0;
	while(s[i] != 0) i++;
	return i;
}
/* Strip off new line from string.
 */
int strstrip(char *s)
{
	int pos = strlen(s);
	while(--pos >= 0)
		if(s[pos] == '\n')
			s[pos] = 0;
	return pos;
}
/* Compares two strings together.
 */
int strcmp(const char *s, const char *t)
{
	int i;
	for(i = 0; s[i] == t[i]; i++) {
		if(s[i] == 0) {
			return 0;
		}
	}
	return s[i]-t[i];
}
/* Reverse a string in place.
 */
void reverse(char *s)
{
	const int len = strlen(s)-1;
	int i, j;
	for(i = 0, j = len; s[i] != s[j]; i++, j--) {
		char tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
}


// /* Convert unsigned integer to c-string.
//  */
// void itoa(unsigned n, unsigned base, char *s, int size)
// {
// 	int opos, pos, top;
	
// 	/* Check bounds */
// 	if(n == 0 || base > 16) {
// 		s[0] = '0';
// 		s[1] = 0;
// 		return;
// 	}
	
// 	/* Fill itoa buffer */
// 	for(pos = 0; n != 0; n /= 10, pos++)
// 		_itoa[pos] = ((char*)_hexdigits)[n % base];
// 	/* Read itoa buffer backwards */
// 	for(opos = 0, top = pos--; opos < size && opos < top; pos--,opos++)
// 		s[opos] = _itoa[pos];
// 	s[opos] = 0;
// }

// /**
//  * Convert unsigned integer to c-string.
//  * @param n           = number to be printed
//  * @param base        = number base for conversion;  decimal=10,hex=16
//  * @param sign        = signed or unsigned output
//  * @param outbuf      = buffer to hold the output number
//  */
// void itoa2(unsigned n, unsigned base, char *outbuf, int size) {
//     int i = 12;
//     int j = 0;

//     do{
//         outbuf[i] = "0123456789ABCDEF"[n % base];
//         i--;
//         n = n / base;
//     } while(n > 0);

//     // if(sign != ' ') {
//     //     outbuf[0] = sign;
//     //     ++j;
//     // }

//     while(++i < 13) {
//        outbuf[j++] = outbuf[i];
//     }

//     outbuf[j] = 0;
// }

unsigned long ultoa(unsigned long value, char* str, unsigned char radix) {
	unsigned long written = 0;
	unsigned int NUMBER_OF_DIGITS = 32;
	unsigned int index;
	unsigned char ch;
	unsigned char buffer[NUMBER_OF_DIGITS];  /* space for NUMBER_OF_DIGITS + '\0' */

	index = NUMBER_OF_DIGITS;

	do {
		ch = '0' + (value % radix);
		if ( ch > (unsigned char)'9') {
			ch += 'a' - '9' - 1;
		}
      buffer[--index] = ch;
      value /= radix;
    } while (value != 0);

	do {
      *str++ = buffer[index++];
	  written++;
    } while (index < NUMBER_OF_DIGITS);

    *str = 0;  /* string terminator */

	return written;
}

long ltoa(long value, char* str, unsigned char radix) {
	if (value < 0 && radix == 10) {
		*str++ = '-';
		value = -value;
	}

	return (long) ultoa((unsigned long) value, str, radix);
}

int itoa_s(int value, unsigned char radix, char *str, int size) {
  return (int) ltoa(value, str, radix);
}

int	atoi(char *str) {
	int neg;
	int num;
	int i;

	i = 0;
	neg = 1;
	num = 0;
	while (str[i] <= ' ')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
		{
			neg *= -1;
		}
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		num = num * 10 + (str[i] - 48);
		i++;
	}
	return (num * neg);
}

void uitoa_s(unsigned int value, unsigned char radix, char *str, int size) {
  ultoa(value, str, radix);
}

void u64toa(uint64_t value, char* str, unsigned char radix) {
	unsigned int NUMBER_OF_DIGITS = 64;
	unsigned int index;
	unsigned char ch;
	unsigned char buffer[NUMBER_OF_DIGITS];  /* space for NUMBER_OF_DIGITS + '\0' */

	index = NUMBER_OF_DIGITS;

	do {
		ch = '0' + (value % radix);
		if ( ch > (unsigned char)'9') {
			ch += 'a' - '9' - 1;
		}
      buffer[--index] = ch;
      value /= radix;
    } while (value != 0);

	do {
      *str++ = buffer[index++];
    } while (index < NUMBER_OF_DIGITS);

    *str = 0;  /* string terminator */
}

int itoa_s2(int value, unsigned char radix, char *str, int size, char leadChar, int leadCount) {
	// uint32_t strOffset = (uint32_t) str;
	int written = itoa_s(value, radix, str, size);
	if (written < leadCount && written < size - 1) {
		// leadCount = 2; written = 1; sizeToCpy = 1;
		int leadSize = leadCount - written;
		memcpy_r(str + (written - 1) + leadSize, str + (written - 1), written);
		memset(str, leadChar, leadSize);
		str[written + leadSize] = '\0';

		written += leadSize;
	}
	return written;
}

/* Convert unsigned integer to c-string safely.
 */
// void itoa_s(int n, unsigned base, char *buf, int size)
// {
// 	if(base > 16) return;
// 	if(n < 0) {
// 		buf[0] = '-';
// 		buf[1] = 0;
// 	}
// 	itoa2(n, base, buf, size);
// }

/* ----------------------- Helper Functions ---------------------- */

/* Sleep for a specific number of ticks.
 */
void delay(uint64_t ticks)
{
	uint64_t eticks = get_timer_ticks() + ticks;
	while(get_timer_ticks() < eticks);
}
/* Sleep for a specific number of seconds.
 */
void sleep(uint64_t seconds)
{
	uint64_t eseconds = get_timer_seconds() + seconds;
	while(get_timer_seconds() < eseconds);
}
/* Append to kernel buffer.
 */
void append(char *s, char c)
{
	int len = strlen(s);
	s[len++] = c;
	s[len] = '\0';
}
/* Backspace from the kernel buffer.
 */
void backspace(char *s)
{
	int len = strlen(s);
	s[len-1] = '\0';
}
