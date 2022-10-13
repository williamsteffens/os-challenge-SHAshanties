/*
Hash Table Implementation - source file
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/sha.h>

#include "hashTable.h"

// TODO: this matches the total amount of reverse hashing requests from client-final
#define TABLE_SIZE 1000



unsigned long hash(uint8_t* key) {
    // djb2 hash function-ish
    unsigned long hash = 5381;

    for (int i = 0; i < 5; ++i)
        hash = ((hash << 5) + hash) + key[i];

    return hash % TABLE_SIZE;
}

entry_t* createEntry(uint8_t* key, uint64_t val) {
    entry_t* entry = (entry_t*) malloc(sizeof(entry_t));
    
    memcpy(entry->key, key, SHA256_DIGEST_LENGTH);
    entry->val = val;

    entry->next = NULL;

    return entry;
}

hashTable_t* htInit() {
    hashTable_t* table = malloc(sizeof(hashTable_t));
    table->entries = calloc(TABLE_SIZE, sizeof(entry_t*));
    for (int i = 0; i < TABLE_SIZE; ++i)
        table->entries[i] = NULL;

    return table;
}

void htSet(hashTable_t* table, uint8_t* key, uint64_t val) {
    // TODO: Insert at the beginning of the list instead, to improve the access time

    int index = hash(key);
    entry_t* entry = table->entries[index];

    if (entry == NULL) {
        table->entries[index] = createEntry(key, val);
        return;
    }

    entry_t* prev; 

    while(entry != NULL) {
        if (memcmp(key, entry->key, SHA256_DIGEST_LENGTH) == 0) {
            entry->val = val;
            return;
        }

        prev = entry;
        entry = entry->next;
    }

    prev->next = createEntry(key, val);

}

uint64_t htGet(hashTable_t* table, uint8_t* key) {
    int index = hash(key);
    entry_t* entry = table->entries[index];

    if (entry == NULL)
        return -1;

    while (entry != NULL) {
        if (memcmp(key, entry->key, SHA256_DIGEST_LENGTH) == 0)   
            return entry->val;
        
        entry = entry->next;
    }

    return -1;
}

void htDelete(hashTable_t* table, uint8_t* key) {
    int index = hash(key);
    entry_t* entry = table->entries[index];

    if (entry == NULL)
        return;

    entry_t* prev = NULL;

    while (entry != NULL) {
        if (memcmp(key, entry->key, SHA256_DIGEST_LENGTH) == 0) {
            if (prev == NULL)
                table->entries[index] = entry->next;
            else
                prev->next = entry->next;
            free(entry);
            return;
        } 

        prev = entry;
        entry = entry->next;
    }
    
}

bool htContainsKey(hashTable_t* table, uint8_t* key) {
    int index = hash(key);
    entry_t* entry = table->entries[index];

    while (entry != NULL) {
        if (memcmp(key, entry->key, SHA256_DIGEST_LENGTH) == 0)   
            return true;
        
        entry = entry->next;
    }
        
    return false;
}

void htDump(hashTable_t* table) {
    entry_t* entry;

    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry = table->entries[i];

        if (entry == NULL)
            continue;

        printf("index[%d]: ", i);

        for(;;) {
            printf("val: %lu", entry->val);

            if (entry->next == NULL)
                break;

            printf(", ");
            entry = entry->next;
        }

        printf("\n");
    }
}

void htFreeTable(hashTable_t* table) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = table->entries[i];
        if (entry != NULL)
            free(entry);
    }
    
    free(table->entries);
    free(table);
}