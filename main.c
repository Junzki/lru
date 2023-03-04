#include <stdio.h>
#include <assert.h>
#include "lru.h"

void lru_display(lru *l) {
    lru_node *ptr = l->head;
    printf("Key\tValue\tAge\n");
    while (NULL != ptr) {
        printf("%s\t%d\t%d\n", ptr->key, ptr->val, ptr->age);
        ptr = ptr->next;
    }
}

int main() {

    lru *lru = lru_new(30);

    lru_node *picked = NULL;

    lru_set(lru, "1", 1);
    lru_set(lru, "2", 1);
    lru_set(lru, "3", 1);

    lru_get(lru, "2");
    lru_display(lru);

    lru_set(lru, "3", 2);
    lru_set(lru, "4", 20);
    lru_display(lru);

    return 0;
}
