

#ifndef LRU_H_
#define LRU_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define LRU_MAP_HASH_SLOTS 4096

typedef struct lru_node {
    struct lru_node *prev;
    struct lru_node *next;
    char *key;
    int val;
    int age;
} lru_node;

typedef lru_node * lru_node_ptr;

typedef struct lru_map_slot {
    char *key;
    int hash;
    lru_node_ptr node;
    struct lru_map_slot *next;
    struct lru_map_slot *prev;
} lru_map_slot;

typedef lru_map_slot * lru_map_slot_ptr;

typedef struct lru_map {
    size_t capacity;
    lru_map_slot_ptr *slots;
} lru_map;

lru_map * lru_map_new(size_t);
void lru_map_set_node(lru_map *, char *, lru_node_ptr);
lru_map_slot_ptr lru_map_get_slot(lru_map *, char *);
void lru_map_delete_slot(lru_map *, lru_map_slot_ptr);

typedef struct lru {
    lru_map *map;
    lru_node *head;
    lru_node *tail;
    int size;
    int capacity;
} lru;

lru * lru_new(int);
void lru_push_node(lru *, lru_node *, int);


void lru_set(lru *, char *, int);
lru_node_ptr lru_get(lru *, char *);

void lru_reshuffle(lru *, lru_node *, int);

int lru_hash(lru_map *, const char *, size_t);


#endif  // LRU_H_