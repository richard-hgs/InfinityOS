#include <stddef.h>
#include <stdint.h>

#include "math.h"

size_t max(size_t val1, size_t val2) {
    return (val1 > val2 ? val1 : val2);
}

size_t min(size_t val1, size_t val2) {
    return (val1 < val2 ? val1 : val2);
}