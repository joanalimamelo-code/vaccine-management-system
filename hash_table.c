/**
 * @file hash_table.c
 * @brief Hash table implementation for user vaccination tracking
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vaccine_system.h"

/**
 * @brief Computes a hash value for a string
 * @param str String to hash
 * @return Hash value
 */
static unsigned hash(const char *str) {
    unsigned hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

/**
 * @brief Creates a new hash table
 * @param size Size of the hash table
 * @return Pointer to the new hash table, or NULL if creation failed
 */
HashTable* create_table(int size) {
    HashTable *table = malloc(sizeof(HashTable));
    if (!table) return NULL;
    table->entries = calloc(size, sizeof(UserEntry*));
    if (!table->entries) { 
        free(table); 
        return NULL; 
    }
    table->size = size;
    table->count = 0;
    return table;
}

/**
 * @brief Frees memory associated with a user entry
 * @param entry Pointer to the user entry
 */
static void free_entry(UserEntry *entry) {
    if (entry) {
        free(entry->indices);
        free(entry->user);
        free(entry);
    }
}

/**
 * @brief Frees all memory associated with a hash table
 * @param table Pointer to the hash table
 */
void free_table(HashTable *table) {
    if (!table) return;
    for (int i = 0; i < table->size; i++) {
        if (table->entries[i]) {
            free_entry(table->entries[i]);
        }
    }
    free(table->entries);
    free(table);
}

/**
 * @brief Finds a user in the hash table
 * @param table Pointer to the hash table
 * @param user User name to find
 * @return Pointer to the user entry if found, NULL otherwise
 */
UserEntry* find_user(HashTable *table, const char *user) {
    if (!table) return NULL;
    unsigned index = hash(user) % table->size;
    for (int i = 0; i < table->size; i++) {
        int idx = (index + i) % table->size;
        if (!table->entries[idx]) return NULL;
        if (!strcmp(table->entries[idx]->user, user)) return table->entries[idx];
    }
    return NULL;
}

/**
 * @brief Creates a new user entry
 * @param user User name
 * @return Pointer to the new user entry, or NULL if creation failed
 */
static UserEntry* create_user_entry(const char *user) {
    UserEntry *new = malloc(sizeof(UserEntry));
    if (!new) return NULL;
    
    new->user = strdup(user);
    if (!new->user) { 
        free(new); 
        return NULL; 
    }
    
    new->indices = malloc(5 * sizeof(int));
    if (!new->indices) { 
        free(new->user); 
        free(new); 
        return NULL; 
    }
    
    new->count = 0;
    new->capacity = 5;
    return new;
}

/**
 * @brief Adds a user to the hash table
 * @param table Pointer to the hash table
 * @param user User name to add
 * @return Pointer to the user entry, or NULL if addition failed
 */
UserEntry* add_user(HashTable *table, const char *user) {
    if (!table || table->count >= table->size * 0.75) return NULL;
    
    unsigned index = hash(user) % table->size;
    for (int i = 0; i < table->size; i++) {
        int idx = (index + i) % table->size;
        if (!table->entries[idx]) {
            UserEntry *new = create_user_entry(user);
            if (!new) return NULL;
            
            table->entries[idx] = new;
            table->count++;
            return new;
        }
        if (!strcmp(table->entries[idx]->user, user)) return table->entries[idx];
    }
    return NULL;
}

/**
 * @brief Adds a vaccination index to a user entry
 * @param user Pointer to the user entry
 * @param index Vaccination index to add
 * @return 1 if successful, 0 if memory allocation failed
 */
int add_vax_index(UserEntry *user, int index) {
    if (!user) return 0;
    if (user->count >= user->capacity) {
        int new_cap = user->capacity * 2;
        int *new = realloc(user->indices, new_cap * sizeof(int));
        if (!new) return 0;
        user->indices = new;
        user->capacity = new_cap;
    }
    user->indices[user->count++] = index;
    return 1;
}
