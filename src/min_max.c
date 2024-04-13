#include "min_max.h"

long min(long a, long b) {
    if (a < b)
        return (a);
    return (b);
}

long max(long a, long b) {
    if (a > b)
        return (a);
    return (b);
}

unsigned long ulmin(unsigned long a, unsigned long b) {
    if (a < b)
        return (a);
    return (b);
}

unsigned long ulmax(unsigned long a, unsigned long b) {
    if (a > b)
        return (a);
    return (b);
}