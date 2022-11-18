#pragma once 

#include <stdbool.h>


typedef struct entry_t {
    uint8_t* key[SHA256_DIGEST_LENGTH];
    uint64_t val;
    struct entry_t* next;
} entry_t;

typedef struct {
    entry_t** entries;
} htable_t; 



htable_t* create_htable();

void htable_set(htable_t* table, uint8_t* key, uint64_t val);

uint64_t htable_get(htable_t* table, uint8_t* key);

void htable_delete(htable_t* table, uint8_t* key);

bool htable_contains_key(htable_t* table, uint8_t* key);

void htable_dump(htable_t* table);

void htable_freeTable(htable_t* table);