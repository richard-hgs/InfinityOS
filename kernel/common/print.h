#ifndef _PRINT_H_
#define _PRINT_H_
#pragma once
/**
 * @param buffer    Output buffer string
 * @param size      The size of the output buffer string
 * @param format    The format to be applied in the string
 * @param ...       The arguments va_args
 * 
 * @return          The length written in the output buffer
 */
int snprintf(char* buffer, size_t size, const char* format, ...);

#endif