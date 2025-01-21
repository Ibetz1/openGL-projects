#include "nstd.hpp"

namespace nstd {

#ifdef STD_HEAP_TRACK

    std::map<U8*, USZ> Mem::heap_used = { };
    USZ Mem::alloc_calls = 0;
    USZ Mem::realloc_calls = 0;
    USZ Mem::free_calls = 0;

    USZ Mem::heap_total() {
        USZ sum = 0;

        for (auto& [key, val] : heap_used) {
            sum += val;
        }

        return sum;
    }

#else

    USZ Mem::heap_total() {
        return 0;
    }

#endif

/*
    fixed allocator impl
*/

// creates a new block of memory
FixedHeapContainer* FixedHeapContainer::create(USZ chunk_count, USZ chunk_size) {
    USZ allocated_space = (chunk_count * chunk_size);
    USZ free_list_space = (chunk_count * sizeof(U8*));

    FixedHeapContainer* block = Mem::type_alloc<FixedHeapContainer>(allocated_space + free_list_space);
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
void FixedHeapContainer::destroy(FixedHeapContainer* block) {
    if (block) {
        Mem::dealloc(block);
    }
}

// returns whether an address is aligned with the memory block
bool FixedHeapContainer::check_addr_alignment(FixedHeapContainer* block, U8* addr) {
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
bool FixedHeapContainer::has_space(FixedHeapContainer* block) {
    return block && block->chunks_free > 0;
}

// inserts a slice of data if it can
U8* FixedHeapContainer::insert(FixedHeapContainer* block, U8* data) {
    USZ* chunks_free = &block->chunks_free;

    if (!(block && *chunks_free)) {
        LOGI("memory block is full");
        return nullptr;
    }

    U8* ref = block->free_list[*chunks_free - 1];
    if (data) {
        Mem::copy(ref, data, block->chunk_size);
    }
    --*chunks_free;

    return ref;
}

// removes a slice of data by reference
void FixedHeapContainer::remove(FixedHeapContainer* block, U8** data, bool assert_aligned) {
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
DynamicHeapContainer* DynamicHeapContainer::create(USZ chunk_count, USZ chunk_size) {
    DynamicHeapContainer* pool = Mem::type_alloc<DynamicHeapContainer>();
    pool->root = FixedHeapContainer::create(chunk_count, chunk_size);
    pool->last = pool->root;
    pool->last_empty = pool->root;
    pool->last_removed = pool->root;
    return pool;
}

// destroys frees existing expanding allocator
void DynamicHeapContainer::destroy(DynamicHeapContainer* pool) {
    if (pool) {
        FixedHeapContainer* fixed = pool->root;

        while (fixed) {
            FixedHeapContainer::destroy(fixed);
            fixed = fixed->next;
        }

        Mem::dealloc(pool);
    }
}

// create a new fixed block on the heap
void DynamicHeapContainer::expand(DynamicHeapContainer* pool) {
    FixedHeapContainer* extended = FixedHeapContainer::create(pool->last->chunk_count, pool->last->chunk_size);
    pool->last->next = extended;
    pool->last = extended;
}

// uses expanding allocator to save data
U8* DynamicHeapContainer::insert(DynamicHeapContainer* pool, U8* data) {
    /*
        insert into discovered block
    */
    FixedHeapContainer* block = find_free_block(pool);
    
    if (!block) {
        return nullptr;
    }

    U8* ref = FixedHeapContainer::insert(block, data);
    return ref;
}

// removes existing reference from expanding allocator
void DynamicHeapContainer::remove(DynamicHeapContainer* pool, U8** data) {
    FixedHeapContainer* cur_block = find_aligned_block(pool, *data);

    if (!cur_block) {
        THROW("data mal aligned");
        return;
    }

    FixedHeapContainer::remove(cur_block, data, true);
    pool->last_removed = cur_block;
}

// returns a free block in the allocator
FixedHeapContainer* DynamicHeapContainer::find_free_block(DynamicHeapContainer* pool) {
    FixedHeapContainer* cur_block = (pool->last_empty) ? pool->last_empty : pool->root;

    if (!FixedHeapContainer::has_space(cur_block)) {
        // if last empty does not have space then find one that does
        cur_block = pool->root;

        while (cur_block) {
            if (FixedHeapContainer::has_space(cur_block)) {
                break;
            }

            cur_block = cur_block->next;
        }

        if (!cur_block) {
            expand(pool);
            return pool->last;
        }

        pool->last_empty = cur_block;
        cur_block = pool->last_empty;
    }

    return cur_block;
}

// finds the block containing a reference
FixedHeapContainer* DynamicHeapContainer::find_aligned_block(DynamicHeapContainer* pool, U8* data) {
    FixedHeapContainer* cur_block = (pool->last_removed) ? pool->last_removed : pool->root;

    if (!FixedHeapContainer::check_addr_alignment(cur_block, data)) {
        cur_block = pool->root;

        while (cur_block) {
            if (cur_block != pool->last_removed && FixedHeapContainer::check_addr_alignment(cur_block, data)) {
                break;
            }

            cur_block = cur_block->next;
        }
    }

    return cur_block;
}

/*
    hash
*/
U32 Hash::generate(const U8* bytes, USZ len, const Hash::Group group) {
    U32 hash = group.seed;

    // traverse single byte chunks
    USZ i = 0;
    while (i < len) {
        U32 v = (U32) *(bytes + i);

        group.func(&hash, v);

        ++i;
    }

    return hash;
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

/*
    pair
*/

USZ Pair::compare_lr(const Pair* a, const Pair* b) {
    USZ alint = (USZ) a->lptr;
    USZ arint = (USZ) a->rptr;
    USZ blint = (USZ) b->lptr;
    USZ brint = (USZ) b->rptr;

    return max(alint, blint) - min(alint, blint) + max(arint, brint) - min(arint, brint);
}

USZ Pair::compare_l(const Pair* a, const Pair* b) {
    USZ alint = (USZ) a->lptr;
    USZ blint = (USZ) b->lptr;

    return max(alint, blint) - min(alint, blint);
}

USZ Pair::compare_r(const Pair* a, const Pair* b) {
    USZ arint = (USZ) a->rptr;
    USZ brint = (USZ) b->rptr;

    return max(arint, brint) - min(arint, brint);
}

/*
    ref table
*/

ReferenceTable* ReferenceTable::create(USZ width, Hash::Group hash_type = Hash::djb2) {
    ReferenceTable* table = Mem::type_alloc<ReferenceTable>();
    table->allocator = Mem::dynamic_container<Item>(width);
    table->references = Data::list<Item*>(width);
    table->hash_type = hash_type;

    return table;
}

void ReferenceTable::destroy(ReferenceTable* table) {
    Mem::dealloc(&table->allocator);
    Mem::dealloc(&table->references);
    Mem::dealloc(table);
}

ReferenceTable::Item* ReferenceTable::get_item(ReferenceTable* table, const U8* ptr) {
    USZ ptr_val = (USZ) ptr;
    USZ hash = Hash::generate((U8*) &ptr_val, sizeof(USZ), table->hash_type) % Data::size(table->references);
    return *Data::at(table->references, hash);
}

void ReferenceTable::insert(ReferenceTable* table, Pair data) {

    // get current item at hash
    USZ ptr_val = (USZ) data.lptr;
    USZ hash = Hash::generate((U8*) &ptr_val, sizeof(USZ), table->hash_type) % Data::size(table->references);
    Item* current = *Data::at(table->references, hash);

    // create a new item
    if (current == nullptr) {
        Item* new_item = Mem::alloc(&table->allocator);
        new_item->region = data;
        new_item->next = nullptr;
        Data::insert(table->references, hash, new_item);
        return;
    }

    while (current) {
        // replace existing value
        if (Pair::compare_l(&current->region, &data) == 0) {
            current->region.rptr = data.rptr;
            return;
        }

        // if we are on the last item in linked list, make a new one
        if (current->next == nullptr) {
            Item* new_item = Mem::alloc(&table->allocator);
            new_item->region = data;
            new_item->next = nullptr;
            current->next = new_item;
            return;
        }

        current = current->next;
    }
}

U8* ReferenceTable::find(ReferenceTable* table, const U8* lptr) {
    Item* current = get_item(table, lptr);

    while (current) {
        Pair* data = &current->region;

        if (data->lptr == lptr) {
            return data->rptr;
        }

        current = current->next;
    }

    return { 0 };
}

void ReferenceTable::iterate(ReferenceTable* table, void (*it)(U8*, U8*)) {
    USZ bin = 0;

    for (USZ bin = 0; bin < Data::size(table->references); ++bin) {
        Item* item = *Data::at(table->references, bin);

        if (!item) {
            continue;
        }

        while (item->next) {
            it(item->region.lptr, item->region.rptr);
            item = item->next;
        }
    }
}

}