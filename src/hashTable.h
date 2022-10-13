#pragma once 

/*
Hash Table Implementation - header file
*/

#include <stdbool.h>

// TODO: this matches the total amount of reverse hashing requests from client-final
#define TABLE_SIZE 1000



typedef struct entry_t {
    uint8_t* key[SHA256_DIGEST_LENGTH];
    uint64_t val;
    struct entry_t* next;
} entry_t;

typedef struct {
    entry_t** entries;
    // TODO: Maybe add this back in, looks better imo
    // int size;
} hashTable_t; 



unsigned long hash(uint8_t* key);
entry_t* createEntry(uint8_t* key, uint64_t val);
hashTable_t* htInit();
void htSet(hashTable_t* table, uint8_t* key, uint64_t val);
uint64_t htGet(hashTable_t* table, uint8_t* key);
void htDelete(hashTable_t* table, uint8_t* key);
bool htContainsKey(hashTable_t* table, uint8_t* key);
void htDump(hashTable_t* table);
void htFreeTable(hashTable_t* table);