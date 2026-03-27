#include "hashtable.h"
#include <stdint.h>
#include <string.h>

#module

// HELPERS

uint16_t generate_hash(char* s) {
    uint32_t hash = 2166136261u; // FNV offset basis
    uint8_t i = 0;
    while (*(s+i) && i < MAX_HASHKEY_SIZE) {
        hash ^= (uint8_t)(*(s + i++));
        hash *= 16777619u;        // FNV prime
    }
    return (uint16_t) (hash % MAX_HASHTABLE_SIZE);
}

// API

void ht_init(hashtable_s* ht) {
    ht->size = 0;
    ht_clear(ht);
}

void ht_clear(hashtable_s* ht) {
    size_t i;
    for (i = 0; i < MAX_HASHTABLE_SIZE; i++) {
        memset(ht->table[i].key, 0, MAX_HASHKEY_SIZE);
        ht->table[i].value = NULL;
        ht->table[i].state = HT_EMPTY; 
    }
}

int1 ht_set(hashtable_s* ht, char* key, void* value) {
    if (ht->size >= MAX_HASHTABLE_SIZE) return 0;
   
    uint16_t h = generate_hash(key);
    uint16_t start = h;
    int32_t first_tombstone = -1;
    while (ht->table[h].state != HT_EMPTY) {
        if (ht->table[h].state == HT_OCCUPIED && strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) == 0) {
            ht->table[h].value = value;
            return 0; 
        }

        if (ht->table[h].state == HT_TOMBSTONE && first_tombstone == -1) {
            first_tombstone = h;
        }

        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) return 0;
    }

    uint16_t insert_pos = (first_tombstone != -1) ? first_tombstone : h;
    strncpy(ht->table[insert_pos].key, key, MAX_HASHKEY_SIZE);
    ht->table[insert_pos].key[MAX_HASHKEY_SIZE - 1] = '\0';
    ht->table[insert_pos].value = value;
    ht->table[insert_pos].state = HT_OCCUPIED;
    ht->size++;
    return 1;
}

int1 ht_delete(hashtable_s* ht, char* key) {
    if (ht->size == 0) return 0;

    uint16_t h = generate_hash(key);
    uint16_t start = h;
    while (ht->table[h].state == HT_TOMBSTONE 
            || (ht->table[h].state == HT_OCCUPIED && strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) != 0)) {
        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) return 0;
    }

    if (ht->table[h].state == HT_EMPTY)
        return 0;

    memset(ht->table[h].key, 0, MAX_HASHKEY_SIZE);
    ht->table[h].value = NULL;
    ht->table[h].state = HT_TOMBSTONE; 
    ht->size--;
    return 1;
}

void* ht_get(hashtable_s* ht, char* key) {
    if (ht->size == 0) return NULL;
    
    uint16_t h = generate_hash(key);
    uint16_t start = h;
    while (ht->table[h].state == HT_TOMBSTONE 
            || (ht->table[h].state == HT_OCCUPIED && strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) != 0)) {
        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) return NULL;
    }

    if (ht->table[h].state == HT_EMPTY)
        return NULL;

    return ht->table[h].value;
}
