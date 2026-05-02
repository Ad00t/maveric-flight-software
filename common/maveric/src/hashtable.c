#include "hashtable.h"
#include "logger.h"
#include "common.h"
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

status_e ht_set(hashtable_s* ht, char* key, uint32_t value) {
    if (ht->size >= MAX_HASHTABLE_SIZE) return FAILURE;
   
    uint16_t h = generate_hash(key);
    uint16_t start = h;
    int32_t first_tombstone = -1;

    while (ht->table[h].state != HT_EMPTY) {
        if (ht->table[h].state == HT_OCCUPIED && strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) == 0) {
            ht->table[h].value = value;
            return FAILURE; 
        }

        if (ht->table[h].state == HT_TOMBSTONE && first_tombstone == -1) {
            first_tombstone = h;
        }

        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) return SUCCESS;
    }

    uint16_t insert_pos = (first_tombstone != -1) ? first_tombstone : h;
    strncpy(ht->table[insert_pos].key, key, MAX_HASHKEY_SIZE);
    ht->table[insert_pos].key[MAX_HASHKEY_SIZE - 1] = '\0';
    ht->table[insert_pos].value = value;
    ht->table[insert_pos].state = HT_OCCUPIED;
    ht->size++;
    // sprintf(LOGBUF, "ht_set: '%s' '%s' %u %u %u %Lu", key, ht->table[insert_pos].key, h, insert_pos, ht->size, value); log_warn();
    return SUCCESS;
}

status_e ht_delete(hashtable_s* ht, char* key) {
    if (ht->size == 0) return FAILURE;

    uint16_t h = generate_hash(key);
    uint16_t start = h;
    while (ht->table[h].state == HT_TOMBSTONE 
            || (ht->table[h].state == HT_OCCUPIED && strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) != 0)) {
        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) return FAILURE;
    }

    if (ht->table[h].state == HT_EMPTY)
        return FAILURE;

    memset(ht->table[h].key, 0, MAX_HASHKEY_SIZE);
    ht->table[h].value = NULL;
    ht->table[h].state = HT_TOMBSTONE; 
    ht->size--;
    return SUCCESS;
}

status_e ht_get(hashtable_s* ht, char* key, uint32_t* out) {
    uint16_t h = generate_hash(key);
    uint16_t start = h;

    while (ht->table[h].state != HT_EMPTY) {
        if (ht->table[h].state == HT_OCCUPIED &&
            strncmp(ht->table[h].key, key, MAX_HASHKEY_SIZE) == 0) {
            *out = ht->table[h].value;
            return SUCCESS;
        }

        h = (h + 1) % MAX_HASHTABLE_SIZE;
        if (h == start) break;
    }

    return FAILURE;
}
