#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <cassert>

#define type_alloc(T) (T*) malloc(sizeof(T))
#define LOGI(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)


/*
    non standard library (nstd) an abstraction over malloc:

    fundamentals:
        pages -> blocks of memory defined either on the stack or the heap
        references -> pointers into a page (basically an index or a pointer)

    structues:
        smart tables -> hash tables of a fixed type where the key is a hash which points to a page (std::map)
        fixed lists -> linked lists of a fixed type where each node points to a page (std::vector)
        strings -> strings that arent c strings :)
*/

struct Node2D {
    Node2D* next;
    Node2D* prev;
    size_t data;

    // creates a new node
    static Node2D create(size_t data) {
        Node2D node = { 0 };

        node.data = data;
        node.next = nullptr;

        return node;
    }

    // destroys nodes entire list
    static void destroy(Node2D* node) {
        if (node == nullptr) {
            return;
        }

        destroy(node->next);
        unlink(node);
    }

    // destroys pref and next references to the node
    static void unlink(Node2D* node) { 
        if (node->prev) {
            node->prev->next = node->next;
            node->prev = nullptr;
        }

        if (node->next) {
            node->next->prev = node->prev;
            node->next = nullptr;
        }
    }

    // find very first node in list
    static Node2D* find_end(Node2D* node) {
        Node2D* cur = node;

        while (cur && cur->next) {
            cur = cur->next;
        }

        return cur;
    }

    // find very last node in list
    static Node2D* find_beg(Node2D* node) {
        Node2D* cur = node;

        while (cur && cur->prev) {
            cur = cur->prev;
        }

        return cur;
    }

    // will stick b on the end of a
    // returns the end of the list
    static Node2D* push_back(Node2D* a, Node2D* b) {
        Node2D* a_end = find_end(a);
        Node2D* b_beg = find_beg(b);

        if (a_end && b_beg) {
            a_end->next = b_beg;
            b_beg->prev = a_end;
        }

        return find_end(b);
    }

    // will stick b in the beginning of a
    // returns the beginning of the list
    static Node2D* push_front(Node2D* a, Node2D* b) {
        Node2D* a_beg = find_beg(a);
        Node2D* b_end = find_end(b);

        if (a_beg && b_end) {
            a_beg->prev = b_end;
            b_end->next = a_beg;
        }

        return find_beg(b);
    }
};

// fixed size block of memory
struct MemBlock {
    MemBlock* next;
    size_t chunks_free;
    size_t chunk_count;
    size_t chunk_size;
    char** free_list;
    char allocated[];

    // creates a new block of memory
    static MemBlock* create(size_t chunk_count, size_t chunk_size) {
        size_t allocated_space = (chunk_count * chunk_size);
        size_t free_list_space = (chunk_count * sizeof(char*));

        MemBlock* block = (MemBlock*) malloc(sizeof(MemBlock) + allocated_space + free_list_space);
        block->free_list = (char**) (block->allocated + allocated_space);

        block->chunks_free = chunk_count;
        block->chunk_count = chunk_count;
        block->chunk_size = chunk_size;
        block->next = nullptr;

        for (int i = 0; i < chunk_count; i++) {
            char* addr = block->allocated + (i * chunk_size);
            block->free_list[i] = addr;
        }

        return block;
    }

    // destroys a mem block and all linked blocks
    static void destroy(MemBlock* block) {
        if (block == nullptr) {
            return;
        }

        destroy(block->next);
        free(block);
    }

    // returns whether an address is aligned with the memory block
    static bool check_addr_alignment(MemBlock* block, char* addr) {
        bool block_exists = block != nullptr;
        bool addr_above_block = addr >= block->allocated;
        bool addr_below_max = addr < block->allocated + (block->chunk_count * block->chunk_size);
        bool addr_chunk_aligned = !((addr - block->allocated) % block->chunk_size);

        return block_exists && addr_above_block && addr_below_max && addr_chunk_aligned;
    }

    static bool has_space(MemBlock* block) {
        return block && block->chunks_free > 0;
    }

    // returns a mem block of the same size which is linked to the current block
    static MemBlock* extend(MemBlock* block) {
        MemBlock* extended = MemBlock::create(block->chunk_count, block->chunk_size);
        block->next = extended;
        return extended;
    }

    // inserts a slice of data if it can
    static char* insert(MemBlock* block, char* data) {
        if (!has_space(block)) {
            LOGE("memory block is full");
            return nullptr;
        }

        char* ref = block->free_list[block->chunks_free - 1];
        memcpy(ref, data, block->chunk_size);
        block->chunks_free--;

        return ref;
    }

    // removes a slice of data by reference
    static void remove(MemBlock* block, char** data, bool assert_aligned = false) {
        if (!assert_aligned && !check_addr_alignment(block, *data)) {
            LOGE("data malaligned");
            return;
        }

        if (block->chunks_free >= block->chunk_count) {
            LOGE("block is already empty");
            return;
        }

        block->free_list[block->chunks_free] = *data;
        block->chunks_free++;

        *data = nullptr;
    }
};

// dynamic array of memory blocks
struct MemPool {
    MemBlock* root;
    MemBlock* last;
    MemBlock* last_empty;
    MemBlock* last_removed;

    static MemPool* create(size_t chunk_count, size_t chunk_size) {
        MemPool* pool = type_alloc(MemPool);
        pool->root = MemBlock::create(chunk_count, chunk_size);
        pool->last = pool->root;
        pool->last_empty = pool->root;
        pool->last_removed = pool->root;
        return pool;
    }

    static void destroy(MemPool* pool) {
        MemBlock::destroy(pool->root);
        free(pool);
    }

    static MemBlock* find_availble_block(MemPool* pool) {
        
        // check if last empty still has space
        MemBlock* cur_block = (pool->last_empty) ? pool->last_empty : pool->root;
        if (MemBlock::has_space(cur_block)) {
            return cur_block;
        }

        // if last empty does not have space then find one that does
        cur_block = pool->root;

        while (cur_block) {
            if (MemBlock::has_space(cur_block)) {
                break;
            }

            cur_block = cur_block->next;
        }

        if (!cur_block) {
            cur_block = MemBlock::extend(pool->last);
            pool->last = cur_block;
        }

        pool->last_empty = cur_block;
        return pool->last_empty;
    }

    static char* insert(MemPool* pool, char* data) {
        MemBlock* slot = find_availble_block(pool);
        return MemBlock::insert(slot, data);
    }

    static void remove(MemPool* pool, char** data) {
        MemBlock* cur_block = (pool->last_removed) ? pool->last_removed : pool->root;

        if (!MemBlock::check_addr_alignment(cur_block, *data)) {
            cur_block = pool->root;

            while (cur_block) {
                if (MemBlock::check_addr_alignment(cur_block, *data)) {
                    break;
                }

                cur_block = cur_block->next;
            }
        }

        if (!cur_block) {
            LOGE("MEMPOOL: data mal aligned");
            return;
        }

        MemBlock::remove(cur_block, data, true);
        pool->last_removed = cur_block;
    }
};

void print_ref(char* ref) {
    if (ref == nullptr) {
        printf("ref is null\n");
        return;
    }

    printf("%i = %i\n", ref, *((int*) ref));

}

#include <bits/stdc++.h> 

int main() {
    size_t allocations = 1000000;
    char** references = (char**) malloc(allocations * sizeof(char*));
    memset(references, 0, allocations * sizeof(char*));

    // heap optimized:   block structure: insertion_time = 0.01634000, free_time = 0.02312000
    // heap unoptimized: block structure: insertion_time = 0.02340000, free_time = 0.03842000

    MemPool* block = MemPool::create(allocations / 500, sizeof(int));

    clock_t start, end;
    double block_insertion_time, block_free_time = 0;
    double alloc_insertion_time, alloc_free_time = 0;
    int count = 50;

    for (int i = 0; i < count ; i++) {
        start = clock();

        for (int i = 0; i < allocations; i++) {
            int test = 12345;
            references[i] = MemPool::insert(block, (char*) &test);
        }

        end = clock();

        block_insertion_time += double(end - start) / double(CLOCKS_PER_SEC);

        start = clock();

        for (int i = 0; i < allocations; i++) {
            MemPool::remove(block, &references[i]);
        }

        end = clock();

        block_free_time += double(end - start) / double(CLOCKS_PER_SEC);


        memset(references, 0, allocations * sizeof(char*));

        start = clock();

        for (int i = 0; i < allocations; i++) {
            int test = 12345;
            char* ref = (char*) malloc(sizeof(int));
            memcpy(ref, &test, sizeof(int));
            references[i] = ref;
        }

        end = clock();

        alloc_insertion_time += double(end - start) / double(CLOCKS_PER_SEC);

        start = clock();

        for (int i = 0; i < allocations; i++) {
            free(references[i]);
        }

        end = clock();

        alloc_free_time += double(end - start) / double(CLOCKS_PER_SEC);
    }

    printf("block structure: insertion_time = %.8f, free_time = %.8f\n", block_insertion_time / count, block_free_time / count);
    printf("alloc structure: insertion_time = %.8f, free_time = %.8f\n", alloc_insertion_time / count, alloc_free_time / count);

    return 0;
}