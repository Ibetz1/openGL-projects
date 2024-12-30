#include "nstd.hpp"

/*
    fixed allocator impl
*/

// creates a new block of memory
FixedAllocator* FixedAllocator::create(size_t chunk_count, size_t chunk_size) {
    size_t allocated_space = (chunk_count * chunk_size);
    size_t free_list_space = (chunk_count * sizeof(char*));

    FixedAllocator* block = Mem::type_alloc<FixedAllocator>(allocated_space + free_list_space);
    block->free_list = (char**) (block->allocated + allocated_space);
    block->chunks_free = chunk_count;
    block->next = nullptr;

    Mem::const_copy(&block->chunk_count, &chunk_count);
    Mem::const_copy(&block->chunk_size, &chunk_size);

    for (int i = 0; i < chunk_count; i++) {
        char* addr = block->allocated + (i * chunk_size);
        block->free_list[i] = addr;
    }

    return block;
}

// destroys a mem block and all linked blocks
void FixedAllocator::destroy(FixedAllocator* block) {
    if (block == nullptr) {
        return;
    }

    size_t allocated_space = (block->chunk_count * block->chunk_size);
    size_t free_list_space = (block->chunk_count * sizeof(char*));

    destroy(block->next);
    Mem::set(block->allocated, 0, allocated_space + free_list_space);
    Mem::dealloc(block);
}

// returns whether an address is aligned with the memory block
bool FixedAllocator::check_addr_alignment(FixedAllocator* block, char* addr) {
    char* allocated = block->allocated;
    size_t chunk_size = block->chunk_size;
    size_t chunk_count = block->chunk_count;

    bool block_exists = block != nullptr;
    bool addr_above_block = addr >= allocated;
    bool addr_below_max = addr < allocated + (chunk_count * chunk_size);
    bool addr_chunk_aligned = !((addr - allocated) % chunk_size);

    return block_exists && addr_above_block && addr_below_max && addr_chunk_aligned;
    // return true;
}

// check if this allocator has space
bool FixedAllocator::has_space(FixedAllocator* block) {
    return block && block->chunks_free > 0;
}

// inserts a slice of data if it can
char* FixedAllocator::insert(FixedAllocator* block, char* data) {
    size_t* chunks_free = &block->chunks_free;

    if (!(block && *chunks_free)) {
        LOGI("memory block is full");
        return nullptr;
    }

    char* ref = block->free_list[*chunks_free - 1];

    Mem::copy(ref, data, block->chunk_size);
    --*chunks_free;

    return ref;
}

// removes a slice of data by reference
void FixedAllocator::remove(FixedAllocator* block, char** data, bool assert_aligned) {
    size_t* chunks_free = &block->chunks_free;

    if (!assert_aligned && !check_addr_alignment(block, *data)) {
        THROW("attempt to remove mal aligned data");
        return;
    }

    if (*chunks_free >= block->chunk_count) {
        THROW("attempt to remove from empty block");
        return;
    }

    *(block->free_list + *chunks_free) = *data;
    ++*chunks_free;

    *data = nullptr;
}

/*
    expanding allocator impl
*/

// creates a new expanding allocator
ExpandingAllocator* ExpandingAllocator::create(size_t chunk_count, size_t chunk_size) {
    ExpandingAllocator* pool = Mem::type_alloc<ExpandingAllocator>();
    pool->root = FixedAllocator::create(chunk_count, chunk_size);
    pool->last = pool->root;
    pool->last_empty = pool->root;
    pool->last_removed = pool->root;
    return pool;
}

// destroys frees existing expanding allocator
void ExpandingAllocator::destroy(ExpandingAllocator* pool) {
    FixedAllocator::destroy(pool->root);
    Mem::dealloc(pool);
}

// create a new fixed block on the heap
void ExpandingAllocator::expand(ExpandingAllocator* pool) {
        FixedAllocator* extended = FixedAllocator::create(pool->last->chunk_count, pool->last->chunk_size);
        pool->last->next = extended;
        pool->last = extended;
}

// uses expanding allocator to save data
char* ExpandingAllocator::insert(ExpandingAllocator* pool, char* data) {
    /*
        insert into discovered block
    */
    FixedAllocator* block = find_free_block(pool);
    
    if (!block) {
        return nullptr;
    }

    char* ref = FixedAllocator::insert(block, data);
    return ref;
}

// removes existing reference from expanding allocator
void ExpandingAllocator::remove(ExpandingAllocator* pool, char** data) {
    FixedAllocator* cur_block = find_aligned_block(pool, *data);

    if (!cur_block) {
        THROW("data mal aligned");
        return;
    }

    FixedAllocator::remove(cur_block, data, true);
    pool->last_removed = cur_block;
}

// returns a free block in the allocator
FixedAllocator* ExpandingAllocator::find_free_block(ExpandingAllocator* pool) {
    FixedAllocator* cur_block = (pool->last_empty) ? pool->last_empty : pool->root;
    if (!FixedAllocator::has_space(cur_block)) {
        // if last empty does not have space then find one that does
        cur_block = pool->root;

        while (cur_block) {
            if (FixedAllocator::has_space(cur_block)) {
                break;
            }

            cur_block = cur_block->next;
        }

        if (!cur_block) {
            expand(pool);
        }

        pool->last_empty = cur_block;
        cur_block = pool->last_empty;
    }

    return cur_block;
}

// finds the block containing a reference
FixedAllocator* ExpandingAllocator::find_aligned_block(ExpandingAllocator* pool, char* data) {
    FixedAllocator* cur_block = (pool->last_removed) ? pool->last_removed : pool->root;

    if (!FixedAllocator::check_addr_alignment(cur_block, data)) {
        cur_block = pool->root;

        while (cur_block) {
            if (cur_block != pool->last_removed && FixedAllocator::check_addr_alignment(cur_block, data)) {
                break;
            }

            cur_block = cur_block->next;
        }
    }

    return cur_block;
}