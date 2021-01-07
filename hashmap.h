#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* structures */

// node struct for the collision resolution linked list
struct hashmap_list_node {
    void* key;
    void* val;
    struct hashmap_list_node* next;
};

// bucket struct for each key/value pair (+ linked list) bucket
struct hashmap_bucket {
    uint64_t hash;
    struct hashmap_list_node* first_node;
};

// the actual map struct
typedef struct _map {
    struct hashmap_bucket* buckets_list;
    size_t key_size;
    size_t data_size;
    int item_count;
    int buckets_list_length;
} * Map;

/* map function primitives */

Map makemap(size_t keysize, size_t valsize);
int map_assign(Map map, void* key, void* val);
void* map_get(Map map, void* key);
void map_remove(Map map, void* key);

/* Other function primitives */

uint64_t FNV1a_hash(void* input, int input_length);
void destroy(Map map);
