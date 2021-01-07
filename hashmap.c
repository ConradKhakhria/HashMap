/* A simple hash map implementation in C
 *
 * This library provides a Map structure and all conceivably necessary
 * functions for interfacing. The keys and values used can be of any
 * size, as the structure will only store pointers to them.
 * 
 * (note: I'm aware that there's a lot of malloc-ing here, and in later
 * editions I will endeavour to remove them)
 */

#include "hashmap.h"

uint64_t FNV1a_hash(void* input, int input_length)
{
    /* FNV-1a hashing algorithm, for internal use */
    uint8_t* byte_array = (uint8_t *)input;
    uint64_t hash = 0xcbf29ce484222325;

    for (int i = 0; i < input_length; i++)
        hash = (hash ^ byte_array[i]) * 0x100000001b3;

    return hash;
}

/* map functions */

Map makemap(size_t keysize, size_t valsize)
{
   /* Returns a fully initialised map object
    * 
    * Parameters
    * ----------
    * - size_t keysize: the size in bytes of the keys to be used
    * - size_t valsize: the size in bytes of the values to be stored
    * 
    * Returns
    * -------
    * A 'Map' object, which is a struct pointer, or NULL if there are any
    * malloc-related errors
    */

    Map returnmap = malloc(sizeof(struct _map));

    if (returnmap == NULL) {
        return NULL;
    }

    returnmap->buckets_list = malloc(sizeof(struct hashmap_bucket) * 0x10000);
    returnmap->buckets_list_length = 0x10000;
    returnmap->key_size   = keysize;
    returnmap->data_size  = valsize;
    returnmap->item_count = 0;

    if (returnmap == NULL) {
        return NULL;
    }

    for (int i = 0; i < 0x10000; i++) {
        returnmap->buckets_list[i].first_node = NULL;
        returnmap->buckets_list[i].hash = 0;
    }

    return returnmap;
}

int map_assign(Map map, void * key, void * val)
{
   /* Assign a key/value pair in the given map
    *
    * Returns
    * -------
    * 0 if this has been completed successfully or if the value
    * was already in the map, -1 if there are malloc-related errors.
    */

    void* valcpy = malloc(map->data_size);
    void* keycpy = malloc(map->key_size);

    if (valcpy == NULL || keycpy == NULL) {
        return -1;
    }

    memcpy(valcpy, val, map->data_size);
    memcpy(keycpy, key, map->key_size);

    struct hashmap_list_node* new_node;
    uint64_t hash  = FNV1a_hash(key, map->key_size);
    int index      = hash % map->buckets_list_length;

    new_node = malloc(sizeof(struct hashmap_list_node));

    if (new_node == NULL) {
        return -1;
    }

    new_node->key  = keycpy;
    new_node->next = NULL;
    new_node->val  = valcpy;

    if (map->buckets_list[index].first_node == NULL) {
        map->buckets_list[index].first_node = new_node;
        map->buckets_list[index].hash = hash;
        map->item_count++;

        return 0;
    } else {
        struct hashmap_list_node* current_node = map->buckets_list[index].first_node;

        while (current_node->next != NULL) {
            if (!strncmp(current_node->key, keycpy, map->key_size)) {
                current_node->val = valcpy;
                return 0;
            } else {
                current_node = current_node->next;
            }
        }

        current_node->next = new_node;
        map->item_count++;
        return 0;
    }
}

void* map_get(Map map, void * key)
{
   /* Get the value associated with a given key
    *
    * Returns
    * -------
    * a pointer to the value stored within the map, or NULL if the
    * map doesn't contain a value associated with the key.
    */

    uint64_t hash = FNV1a_hash(key, map->key_size);
    int index     = hash % map->buckets_list_length;
    struct hashmap_list_node* current_node = map->buckets_list[index].first_node;

    while (current_node != NULL) {
        if (!strncmp(current_node->key, key, map->key_size)) {
            return current_node->val;
        } else {
            current_node = current_node->next;
        }
    }

    return NULL;
}

void map_remove(Map map, void* key)
{
   /* Remove a key/value pair from the map 
    *
    * Returns
    * -------
    * Nothing.
    */

    uint64_t hash = FNV1a_hash(key, map->key_size);
    int index     = hash % map->buckets_list_length;
    struct hashmap_list_node* current_node = map->buckets_list[index].first_node;
    struct hashmap_list_node* prev_node    = NULL;

    while (current_node != NULL) {
        if (!strncmp(current_node->key, key, map->key_size)) {
            if (prev_node == NULL) {
                map->buckets_list[index].first_node = current_node->next;
            } else {
                prev_node->next = current_node->next;
            }

            free(current_node->key);
            free(current_node->val);
            free(current_node);

            return;
        } else {
            prev_node    = current_node;
            current_node = current_node->next;
        }
    }
}

void map_destroy(Map map)
{
    /* Free's all memory allocated for the map */
    struct hashmap_list_node* current_node, *prev_node;

    for (int i = 0; i < map->buckets_list_length; i++) {
        current_node = map->buckets_list[i].first_node;

        while (current_node != NULL) {
            prev_node    = current_node;
            current_node = current_node->next;

            free(prev_node->key);
            free(prev_node->val);
            free(prev_node);
        }
    }

    free(map->buckets_list);
    free(map);
}
