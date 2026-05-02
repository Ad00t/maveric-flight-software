#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdint.h>

#define MAX_HASHKEY_SIZE    20 
#define MAX_HASHTABLE_SIZE  64 

typedef enum {
    HT_EMPTY,
    HT_OCCUPIED,
    HT_TOMBSTONE
} ht_state_e;

typedef struct {
    char key[MAX_HASHKEY_SIZE];
    uint32_t value;
    ht_state_e state;
} ht_item_s;

typedef struct {
    uint8_t size;
    ht_item_s table[MAX_HASHTABLE_SIZE];
} hashtable_s;

// Initialize a hashtable
void ht_init(hashtable_s* ht);

// Clear all items from hashtable
void ht_clear(hashtable_s* ht);

// Set a key in the table to the provided value, or insert if nonexistent, return 1 if succeeded, 0 if otherwise
status_e ht_set(hashtable_s* ht, char* key, uint32_t value); // had to change from void* to cmdimpl_f due to addr truncation

// Delete a key from the hashtable, return 1 if succeeded, 0 if otherwise
status_e ht_delete(hashtable_s* ht, char* key);

// Get data at the specified key in the table, or NULL if nonexistent
status_e ht_get(hashtable_s* ht, char* key, uint32_t* out);

#endif
