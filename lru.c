
#include "lru.h"

lru_map *
lru_map_new(size_t capacity) {
    lru_map *m = NULL;

    if (NULL == (m = (lru_map *) malloc(sizeof(lru_map))))
        exit(EXIT_FAILURE);

    m->capacity = capacity;
    if (NULL == (m->slots = (lru_map_slot_ptr *) malloc(sizeof(lru_map_slot_ptr) * capacity)))
        exit(EXIT_FAILURE);

    memset(m->slots, 0, sizeof(lru_map_slot_ptr) * capacity);
    return m;
}

void
lru_map_set_node(lru_map *m, char *key, lru_node_ptr node) {
    size_t size = strlen(key);
    int hash = lru_hash(m, key, size);

    lru_map_slot_ptr s = (lru_map_slot_ptr) malloc(sizeof(lru_map_slot));
    s->key = key;
    s->hash = hash;
    s->node = node;
    s->prev = NULL;
    s->next = NULL;

    if (NULL == m->slots[hash]) {
        m->slots[hash] = s;
        return;
    }

    lru_map_slot_ptr e = m->slots[hash];
    s->next = e;
    e->prev = s;
}

lru_map_slot_ptr
lru_map_get_slot(lru_map *m, char *key) {
    size_t size = strlen(key);
    int hash = lru_hash(m, key, size);

    lru_map_slot_ptr s = m->slots[hash];
    while (NULL != s) {
        if (0 == strcmp(key, s->key)) {
            return s;
        }

        s = s->next;
    }

    return NULL;
}

void
lru_map_delete_slot(lru_map *m, lru_map_slot_ptr s) {
    if (NULL == s)
        return;

    lru_map_slot_ptr next = s->next;
    lru_map_slot_ptr prev = s->prev;

    if (NULL == next && NULL == prev)
        m->slots[s->hash] = NULL;

    if (NULL != prev) {
        prev->next = next;
    }

    if (NULL != next) {
        next->prev = prev;
    }

    free(s);
}


lru *
lru_new(int capacity) {
    lru * l = (lru *) malloc(sizeof(lru));
    if (NULL == l)
        exit(EXIT_FAILURE);

    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
    l->capacity = capacity;

    lru_map *map = lru_map_new(LRU_MAP_HASH_SLOTS);
    l->map = map;

    return l;
}


void
lru_tick(lru *lru) {
    lru_node *ptr = lru->head;
    while (NULL != ptr) {
        ptr->age++;
        ptr = ptr->next;
    }
}

void
lru_reshuffle(lru *lru, lru_node *picked, int tick) {
    lru_node *prev = picked->prev;
    lru_node *next = picked->next;

    if (NULL != prev)
        prev->next = next;

    if (NULL != next)
        next->prev = prev;
    else {
        // If picked is tail
        lru->tail = prev;
    }

    lru_push_node(lru, picked, tick);
}

lru_node_ptr
lru_get(lru *lru, char *key) {
    lru_map_slot_ptr s = lru_map_get_slot(lru->map, key);
    if (NULL == s)
        return NULL;

    lru_node_ptr picked = s->node;

    picked->age = 0;
    lru_reshuffle(lru, picked, 1);

    return picked;
}

void
lru_set(lru* lru, char *key, int val) {
    // Clean expired items
    while (lru->size >= lru->capacity) {
        lru_node *tail = lru->tail;

        lru->tail = lru->tail->prev;
        lru->tail->next = NULL;
        lru->size--;

        lru_map_slot_ptr s = lru_map_get_slot(lru->map, tail->key);
        lru_map_delete_slot(lru->map, s);

        free(tail);
    }

    lru_node *node = NULL;
    lru_map_slot_ptr e = lru_map_get_slot(lru->map, key);
    if (NULL != e) {
        node = e->node;
    } else {
        if (NULL == (node = (lru_node *) malloc(sizeof(lru_node))))
            exit(EXIT_FAILURE);

        lru->size++;
        memset(node, 0, sizeof(lru_node));
        lru_map_set_node(lru->map, key, node);
    }

    node->key = key;
    node->val = val;
    node->age = 0;

    lru_reshuffle(lru, node, 1);
}


void
lru_push_node(lru *lru, lru_node *node, int tick) {
    if (NULL == lru->head) {
        // New LRU list.
        lru->head = node;
        lru->tail = node;
        return;
    }

    node->next = lru->head;
    node->prev = NULL;

    if (NULL != lru->head)
        lru->head->prev = node;

    if (1 == tick)
        lru_tick(lru);

    lru->head = node;
}
