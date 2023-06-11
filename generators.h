#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include "nullCheck.h"


#define DYNARR_GEN(type, name, termElement, addField)   \
typedef struct {                                        \
    type * array;                                       \
    size_t _capacity;                                   \
    size_t len;                                         \
    addField;                                           \
} name;                                                 \
                                                        \
void initEmpty##name(name* p) {                         \
    type* arr = (type* )memalloc(sizeof(type) * 2);     \
    nullCheck(arr);                                     \
    arr[0] = (type)termElement;                         \
    p->_capacity = 1;                                   \
    p->len = 0;                                         \
    p->array  = arr;                                    \
}                                                       \
                                                        \
void append##name(name* p, type elem) {                 \
    if (p->_capacity <= p->len) {                       \
        p->_capacity *= 2;                              \
        p->array = (type* )memrealloc(                  \
            p->array,                                   \
            sizeof(type) * (p->_capacity + 1)           \
        );                                              \
        nullCheck(p->array);                            \
    }                                                   \
    p->array[(p->len)++] = elem;                        \
    p->array[p->len] = (type)termElement;               \
}                                                       \
                                                        \
type pop##name(name* p, size_t ind) {                   \
    assert(ind < p->len);                               \
    type elem = (p->array)[ind];                        \
    for (size_t i = ind + 1; i <= p->len; i++) {        \
        (p->array)[i - 1] = (p->array)[i];              \
    }                                                   \
    (p->len)--;                                         \
    return elem;                                        \
}


#define DYNARR_FREE_FUNC_GEN(typeName, elemFreeFunc)    \
void free##typeName(typeName* p) {                      \
    for (size_t i = 0; i < (p->len); i++) {             \
        elemFreeFunc((p->array) + i);                   \
    }                                                   \
    memfree(p->array);                                  \
    p->_capacity = 0;                                   \
    p->len = 0;                                         \
}
