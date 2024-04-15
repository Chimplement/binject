#include <string.h>
#include <stdlib.h>

#include "str_manip.h"

char* str_join(const char* str1, const char* str2) {
    char* new_str = malloc(strlen(str1) + strlen(str2) + 1);
    if (new_str == NULL)
        return (NULL);
    strcpy(new_str, str1);
    strcpy(new_str + strlen(str1), str2);
    return (new_str);
}