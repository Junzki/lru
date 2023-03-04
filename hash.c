

#include "lru.h"


int
lru_hash(lru_map *map, const char *in__, size_t size) {
    int result = -1;

    if (NULL == in__)
        return result;

    size_t i;
    for(i = 0; i < size; i++) {
        result += (char)in__[i];
    }

    result = result % (int) map->capacity;
    return result;
}
