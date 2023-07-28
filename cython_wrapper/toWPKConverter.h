#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
#include <dwmapi.h>
#include <stdio.h>
#include "dirent/include/dirent.h"


#define add_object(list, object)                                                                                                        \
do {                                                                                                                                    \
    if ((list)->length == (list)->allocated_length) {                                                                                   \
        (list)->objects = realloc((list)->objects, ((list)->allocated_length + ((list)->allocated_length >> 1)) * sizeof(*object));     \
        (list)->allocated_length += (list)->allocated_length >> 1;                                                                      \
    }                                                                                                                                   \
                                                                                                                                        \
    (list)->objects[(list)->length] = *object;                                                                                          \
    (list)->length++;                                                                                                                   \
} while (0);            

#define initialize_list_size(list, size)                                                                                                \
do {                                                                                                                                    \
    (list)->length = 0;                                                                                                                 \
    (list)->allocated_length = size;                                                                                                    \
    (list)->objects = malloc((size) * sizeof((list)->objects[0]));                                                                      \
} while (0)                                                                                                                             \
