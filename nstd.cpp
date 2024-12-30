#include "nstd.hpp"

namespace nstd {

/*
    fixed allocator impl
*/

// creates a new block of memory
FixedAllocator* FixedAllocator::create(USZ chunk_count, USZ chunk_size) {
    USZ allocated_space = (chunk_count * chunk_size);
    USZ free_list_space = (chunk_count * sizeof(U8*));

    FixedAllocator* block = Mem::type_alloc<FixedAllocator>(allocated_space + free_list_space);
    block->free_list = (U8**) (block->allocated + allocated_space);
    block->chunks_free = chunk_count;
    block->next = nullptr;

    Mem::const_copy(&block->chunk_count, &chunk_count);
    Mem::const_copy(&block->chunk_size, &chunk_size);

    for (int i = 0; i < chunk_count; i++) {
        U8* addr = block->allocated + (i * chunk_size);
        block->free_list[i] = addr;
    }

    return block;
}

// destroys a mem block and all linked blocks
void FixedAllocator::destroy(FixedAllocator* block) {
    if (block == nullptr) {
        return;
    }

    USZ allocated_space = (block->chunk_count * block->chunk_size);
    USZ free_list_space = (block->chunk_count * sizeof(U8*));

    destroy(block->next);
    Mem::set(block->allocated, 0, allocated_space + free_list_space);
    Mem::dealloc(block);
}

// returns whether an address is aligned with the memory block
bool FixedAllocator::check_addr_alignment(FixedAllocator* block, U8* addr) {
    if (AUTO_ASSERT_MEMORY_ALIGNED) {
        return true;
    }

    U8* allocated = block->allocated;
    USZ chunk_size = block->chunk_size;
    USZ chunk_count = block->chunk_count;

    bool block_exists = block != nullptr;
    bool addr_above_block = addr >= allocated;
    bool addr_below_max = addr < allocated + (chunk_count * chunk_size);
    bool addr_chunk_aligned = !((addr - allocated) % chunk_size);

    return block_exists && addr_above_block && addr_below_max && addr_chunk_aligned;
}

// check if this allocator has space
bool FixedAllocator::has_space(FixedAllocator* block) {
    return block && block->chunks_free > 0;
}

// inserts a slice of data if it can
U8* FixedAllocator::insert(FixedAllocator* block, U8* data) {
    USZ* chunks_free = &block->chunks_free;

    if (!(block && *chunks_free)) {
        LOGI("memory block is full");
        return nullptr;
    }

    U8* ref = block->free_list[*chunks_free - 1];

    Mem::copy(ref, data, block->chunk_size);
    --*chunks_free;

    return ref;
}

// removes a slice of data by reference
void FixedAllocator::remove(FixedAllocator* block, U8** data, bool assert_aligned) {
    USZ* chunks_free = &block->chunks_free;

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
ExpandingAllocator* ExpandingAllocator::create(USZ chunk_count, USZ chunk_size) {
    ExpandingAllocator* pool = Mem::type_alloc<ExpandingAllocator>();
    pool->root = FixedAllocator::create(chunk_count, chunk_size);
    pool->last = pool->root;
    pool->last_empty = pool->root;
    pool->last_removed = pool->root;
    return pool;
}

// destroys frees existing expanding allocator
void ExpandingAllocator::destroy(ExpandingAllocator* pool) {
    if (pool) {
        FixedAllocator::destroy(pool->root);
        Mem::dealloc(pool);
    }
}

// create a new fixed block on the heap
void ExpandingAllocator::expand(ExpandingAllocator* pool) {
        FixedAllocator* extended = FixedAllocator::create(pool->last->chunk_count, pool->last->chunk_size);
        pool->last->next = extended;
        pool->last = extended;
}

// uses expanding allocator to save data
U8* ExpandingAllocator::insert(ExpandingAllocator* pool, U8* data) {
    /*
        insert into discovered block
    */
    FixedAllocator* block = find_free_block(pool);
    
    if (!block) {
        return nullptr;
    }

    U8* ref = FixedAllocator::insert(block, data);
    return ref;
}

// removes existing reference from expanding allocator
void ExpandingAllocator::remove(ExpandingAllocator* pool, U8** data) {
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
FixedAllocator* ExpandingAllocator::find_aligned_block(ExpandingAllocator* pool, U8* data) {
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

/*
    fixed array
*/

FixedArray* FixedArray::create(USZ elements, USZ element_size) {
    USZ size = elements * element_size;

    FixedArray* array = Mem::type_alloc<FixedArray>(size);
    Mem::const_copy(&array->size_bytes, &size);
    Mem::const_copy(&array->elements, &elements);
    Mem::const_copy(&array->element_size, &element_size);

    Mem::set(array->data, 0, size);

    return array;
}

FixedArray* FixedArray::create(const U8* buffer, USZ elements, USZ element_size) {
    USZ size = elements * element_size;

    FixedArray* array = Mem::type_alloc<FixedArray>(size);
    Mem::const_copy(&array->size_bytes, &size);
    Mem::const_copy(&array->elements, &elements);
    Mem::const_copy(&array->element_size, &element_size);

    Mem::copy(array->data, buffer, size);

    return array;
}

void FixedArray::destroy(FixedArray* array) {
    if (array) {
        Mem::dealloc(array);
    }
}

U8* FixedArray::begin(FixedArray* array) {
    return array->data;
}

U8* FixedArray::end(FixedArray* array) {
    return array->data + array->size_bytes;
}

U8* FixedArray::at(FixedArray* array, USZ idx) {
    if (idx >= array->elements) {
        THROW("index out of bounds");
        return nullptr;
    }

    return array->data + idx * array->element_size;
}

void FixedArray::insert(FixedArray* array, USZ idx, U8* data) {
    if (idx >= array->elements) {
        THROW("index out of bounds");
        return;
    }

    Mem::copy(array->data + idx * array->element_size, data, array->element_size);
}

void FixedArray::copy(FixedArray* a, FixedArray* b) {
    if (a->elements != b->elements || a->element_size != b->element_size) {
        THROW("cannot copy mal aligned arrays");
        return;
    }

    Mem::copy(a->data, b->data, a->size_bytes);
}

bool FixedArray::equals(FixedArray* a, FixedArray* b) {
    if (a->elements != b->elements || a->element_size != b->element_size) {
        return false;
    }

    return Mem::compare(a->data, b->data, a->size_bytes) == 0;
}

USZ FixedArray::size(FixedArray* array) {
    return array->elements;
}

void FixedArray::clear(FixedArray* array) {
    Mem::set(array->data, 0, array->size_bytes);
}

/*
    expanding array
*/

ExpandingArray* ExpandingArray::create(USZ element_size, USZ reserved) {
    ExpandingArray* array = Mem::type_alloc<ExpandingArray>();
    array->data = Mem::alloc<U8>(element_size * reserved);
    Mem::set(array->data, 0, element_size * reserved);
    Mem::const_copy(&array->element_size, &element_size);
    Mem::const_copy(&array->reserved, &reserved);
    array->max_elements = reserved;
    array->elements_used = 0;
    return array;
}

ExpandingArray* ExpandingArray::create(const U8* buffer, USZ element_size, USZ num_elements) {
    ExpandingArray* array = create(element_size, num_elements);
    Mem::copy(array->data, buffer, element_size * num_elements);
    array->elements_used = num_elements;
    return array;
}

void ExpandingArray::destroy(ExpandingArray* array) {
    if (array) {
        Mem::dealloc(array->data);
        Mem::dealloc(array);
    }
}

void ExpandingArray::expand(ExpandingArray* array) {
    USZ new_size = array->max_elements * 2;
    array->data = Mem::resize(array->data, new_size * array->element_size);
    array->max_elements = new_size;
}

void ExpandingArray::expand(ExpandingArray* array, USZ size) {
    USZ new_size = array->max_elements + size;
    array->data = Mem::resize(array->data, new_size * array->element_size);
    array->max_elements = new_size;
}

void ExpandingArray::join_raw(ExpandingArray* array, const U8* data, USZ size) {
    USZ new_size = array->elements_used + size;

    if (new_size > array->max_elements) {
        expand(array, new_size - array->max_elements);
    }

    Mem::copy(array->data + array->elements_used * array->element_size, data, size);
}

void ExpandingArray::shrink(ExpandingArray* array) {
    USZ new_size = (array->elements_used) + 1;

    if (new_size < array->reserved) {
        new_size = array->reserved;
    }

    if (new_size == array->max_elements) {
        return;
    }

    array->data = Mem::resize(array->data, new_size * array->element_size);
    array->max_elements = new_size;
}

void ExpandingArray::push_back(ExpandingArray* array, U8* val) {
    if (array->elements_used - array->max_elements == 0) {
        expand(array);
    }

    memcpy(array->data + array->elements_used * array->element_size, val, array->element_size);
    array->elements_used++;
}

void ExpandingArray::pop_back(ExpandingArray* array) {
    array->elements_used--;
}

U8* ExpandingArray::get(ExpandingArray* array, USZ idx) {
    return array->data + idx * array->element_size;
}

void ExpandingArray::set(ExpandingArray* array, USZ idx, U8* val) {
    if (idx >= array->elements_used) {
        THROW("index out of bounds");
    }

    Mem::copy(get(array, idx), val, array->element_size);
}

U8* ExpandingArray::at(ExpandingArray* array, USZ idx) {
    if (idx >= array->elements_used) {
        THROW("index out of bounds");
    }

    return get(array, idx);
}

void ExpandingArray::insert(ExpandingArray* array, USZ idx, U8* val) {
    if (idx >= array->elements_used) {
        THROW("index out of bounds");
        return; // error
    }

    if (array->elements_used == array->max_elements) {
        expand(array);
    }

    Mem::copy(
        get(array, idx + 1),
        get(array, idx),
        (array->elements_used - idx) * array->element_size
    );

    Mem::copy(get(array, idx), val, array->element_size);
    array->elements_used++;
}

void ExpandingArray::erase(ExpandingArray* array, USZ idx) {
    if (idx >= array->elements_used) {
        THROW("index out of bounds");
        return; // error
    }

    if (array->elements_used == array->max_elements) {
        expand(array);
    }

    Mem::copy(
        get(array, idx),
        get(array, idx + 1),
        (array->elements_used - idx) * array->element_size
    );

    array->elements_used--;
}

U8* ExpandingArray::begin(ExpandingArray* array) {
    if (array->elements_used == 0) {
        return nullptr;
    }

    return array->data;
}

U8* ExpandingArray::end(ExpandingArray* array) {
    if (array->elements_used == 0) {
        return nullptr;
    }

    return array->data + (array->elements_used - 1) * array->element_size;
}

USZ ExpandingArray::size(ExpandingArray* array) {
    return array->elements_used;
}

USZ ExpandingArray::capacity(ExpandingArray* array) {
    return array->max_elements;
}

USZ ExpandingArray::empty(ExpandingArray* array) {
    return array->elements_used == 0;
}

void ExpandingArray::clear(ExpandingArray* array) {
    array->elements_used = 0;
    shrink(array);
}

bool ExpandingArray::equals(ExpandingArray* a, ExpandingArray* b) {
    if (a->elements_used != b->elements_used || a->element_size != b->element_size) {
        return false;
    }

    return Mem::compare(a->data, b->data, b->elements_used * a->element_size) == 0;
}

}