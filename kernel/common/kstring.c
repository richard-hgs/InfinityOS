
#include <stdint.h>
#include "kstring.h"

size_t kstrlen(const char *s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

size_t kstrlen_offset(const char *s, size_t startOffset) {
   size_t n = startOffset;
   while (s[n]) ++n;
   return n;
}

void kstr_toupper(char* str, size_t size) {
   for (size_t i = 0; str[i] !='\0' && i < size; i++) {
      if(str[i] >= 'a' && str[i] <= 'z') {
         str[i] = str[i] -32;
      }
   }
}

int kstrcmp(const char *s1, const char *s2) {
    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int kstrncmp(const char *s1, const char *s2, size_t n) {
   while (n != 0 && *s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
      s1++;
      s2++;
      n--;
   }
   return n == 0 ? 0 : (*s1 - *s2);
}

int kmemcmp(const void* b1, const void* b2, size_t n) {
   uint8_t *p1 = (uint8_t*) b1;
   uint8_t *p2 = (uint8_t*) b2;
   while (n > 0 && *p1 == *p2) {
      p1++;
      p2++;
      n--;
   }
   return n == 0 ? 0 : (*p1 - *p2);
}