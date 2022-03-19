#ifndef __KSTRING_H__
#define __KSTRING_H__
#pragma once

#include <stddef.h>

size_t kstrlen(const char *s);

size_t kstrlen_offset(const char *s, size_t startOffset);

void kstr_toupper(char* str, size_t size);

int kstrcmp(const char* s1, const char* s2);

int kstrncmp(const char* s1, const char* s2, size_t n);

#endif