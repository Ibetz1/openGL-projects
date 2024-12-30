#ifndef _NSTD_HPP
#define _NSTD_HPP

#include <iostream>
#include <cstring>
#include <stdint.h>

#define LOGI(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define THROW(fmt, ...) do { LOGE(__FILE__ fmt, ##__VA_ARGS__); exit(1); } while (0)

// fixed size block of memory
struct FixedAllocator {
    FixedAllocator* next;
    size_t chunks_free;
    const size_t chunk_count;
    const size_t chunk_size;
    char** free_list;
    char allocated[];

    // creates a new block of memory
    static FixedAllocator* create(size_t chunk_count, size_t chunk_size);

    // destroys a mem block and all linked blocks
    static void destroy(FixedAllocator* block);

    // inserts a slice of data if it can
    static char* insert(FixedAllocator* block, char* data);

    // removes a slice of data by reference
    static void remove(FixedAllocator* block, char** data, bool assert_aligned = false);

    // returns whether an address is aligned with the memory block
    static bool check_addr_alignment(FixedAllocator* block, char* addr);

    // check if fixed allocator has space
    static bool has_space(FixedAllocator* block);
};

// dynamic array of memory blocks
struct ExpandingAllocator {
    FixedAllocator* root;
    FixedAllocator* last;
    FixedAllocator* last_empty;
    FixedAllocator* last_removed;

    // create a new expanding allocator, this is like a fixed allocator but with no maximum size
    static ExpandingAllocator* create(size_t chunk_count, size_t chunk_size);

    // destroy an expanding allocator
    static void destroy(ExpandingAllocator* pool);

    // conactinates a new pool onto the allocator
    static void expand(ExpandingAllocator* pool);

    // insert into the allocator
    static char* insert(ExpandingAllocator* pool, char* data);

    // remove from the allocator
    static void remove(ExpandingAllocator* pool, char** data);

    // returns a free block in the allocator
    static FixedAllocator* find_free_block(ExpandingAllocator* pool);

    // finds the block containing a reference
    static FixedAllocator* find_aligned_block(ExpandingAllocator* pool, char* reference);
};

template <typename T, typename A>
struct Block {
    A* allocator;
};

template <typename T, typename A = FixedAllocator> using FixedBlock = Block<T, A>;
template <typename T, typename A = ExpandingAllocator> using DynamicBlock = Block<T, A>;

// wrapper of malloc for profiling
struct Mem {

    /*
        allocate to buffer
    */

    template <typename T = size_t>
    inline static Block<T, FixedAllocator> fixed(size_t size) {
        Block<T, FixedAllocator> memory = { 0 };
        memory.allocator = FixedAllocator::create(size, sizeof(T));
        return memory;
    }

    template <typename T = size_t>
    inline static Block<T, ExpandingAllocator> dynamic(size_t size) {
        Block<T, ExpandingAllocator> memory = { 0 };
        memory.allocator = ExpandingAllocator::create(size, sizeof(T));
        return memory;
    }

    template <typename T = size_t, typename A>
    inline static T* alloc(Block<T, A>* block, T v) {
        return (T*) A::insert(block->allocator, (char*) &v);
    }

    template <typename T = size_t, typename A>
    inline static T* alloc(Block<T, A>* block, T* v) {
        return (T*) A::insert(block->allocator, (char*) v);
    }

    template <typename T = size_t, typename A>
    inline static void dealloc(Block<T, A>* block, T** ref) {
        A::remove(block->allocator, (char**) ref);
    }

    template <typename T = size_t, typename A>
    inline static void dealloc(Block<T, A>* block) {
        A::destroy(block->allocator);
        *block = { 0 };
    }

    /*
        base allocation calls
    */

    template <typename T = void>
    inline static T* alloc(size_t size) { 
        T* ret = (T*) malloc(size);
        return ret;
    }
    
    template <typename T = void>
    inline static T* alloc(size_t count, size_t size) { 
        T* ret = (T*) calloc(count, size);
        return (T*) calloc(count, size); 
    }

    template <typename T = void>
    inline static void dealloc(T* ptr) {
        free(ptr); 
    }

    template <typename T>
    static T* resize(T* ptr, size_t size) {
        return (T*) realloc(ptr, size);
    }

    template <typename T = void>
    inline static T* type_alloc(size_t padding = 0) {
        return alloc<T>(sizeof(T) + padding);
    }

    template <typename B = void, typename A = void>
    inline static void copy(B* dst, A* ptr, size_t size) {
        memcpy(dst, ptr, size);
    }

    template <typename B = void, typename A = void>
    inline static void copy(B* dst, A* ptr) {
        memcpy(dst, ptr, sizeof(A));
    }

    template <typename B = void, typename A = void>
    inline static void const_copy(const B* dst, const A* ptr) {
        memcpy((B*) dst, (A*) ptr, sizeof(A));
    }

    template <typename T = void>
    inline static void set(T* dst, int val, size_t size) {
        memset(dst, val, size);
    }
};

#endif