#ifndef _NSTD_HPP
#define _NSTD_HPP

#include <iostream>
#include <cstring>
#include <stdint.h>

#define LOGI(fmt, ...) do { printf(fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE(fmt, ...) do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while (0)
#define THROW(fmt, ...) do { LOGE(__FILE__ " " fmt, ##__VA_ARGS__); exit(1); } while (0)

namespace nstd {

typedef size_t USZ;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t SS8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef float F32;
typedef double F64;

template <typename T, typename A>
struct Block {
    A* structure;

    struct Temp;
};

template <typename T, typename A>
struct Block<T, A>::Temp : Block<T, A> {

    Temp(const Block<T, A>& arr) {
        this->structure = arr.structure;
    }

    Temp(Block<T, A>&& arr) noexcept {
        this->structure = arr.structure;
        arr.structure = nullptr;
    }

    ~Temp() {
        A::destroy(this->structure);
        this->structure = nullptr;
    }

};

/*
    memory
*/

// fixed size block of memory
struct FixedAllocator {

    #ifndef AUTO_ASSERT_MEMORY_ALIGNED
        // enables auto alignment assertion (~5-10% speed gain, but removes safety)
        static constexpr bool AUTO_ASSERT_MEMORY_ALIGNED = false;
    #endif

    FixedAllocator* next;
    USZ chunks_free;
    const USZ chunk_count;
    const USZ chunk_size;
    U8** free_list;
    U8 allocated[];

    // creates a new block of memory
    static FixedAllocator* create(USZ chunk_count, USZ chunk_size);

    // destroys a mem block and all linked blocks
    static void destroy(FixedAllocator* block);

    // inserts a slice of data if it can
    static U8* insert(FixedAllocator* block, U8* data);

    // removes a slice of data by reference
    static void remove(FixedAllocator* block, U8** data, bool assert_aligned = AUTO_ASSERT_MEMORY_ALIGNED);

    // returns whether an address is aligned with the memory block
    static bool check_addr_alignment(FixedAllocator* block, U8* addr);

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
    static ExpandingAllocator* create(USZ chunk_count, USZ chunk_size);

    // destroy an expanding allocator
    static void destroy(ExpandingAllocator* pool);

    // conactinates a new pool onto the allocator
    static void expand(ExpandingAllocator* pool);

    // insert into the allocator
    static U8* insert(ExpandingAllocator* pool, U8* data);

    // remove from the allocator
    static void remove(ExpandingAllocator* pool, U8** data);

    // returns a free block in the allocator
    static FixedAllocator* find_free_block(ExpandingAllocator* pool);

    // finds the block containing a reference
    static FixedAllocator* find_aligned_block(ExpandingAllocator* pool, U8* reference);
};

// wrapper of malloc for profiling
struct Mem {

    /*
        allocate to buffer
    */

    template <typename T = USZ>
    inline static Block<T, FixedAllocator> fixed(USZ size) {
        Block<T, FixedAllocator> memory = { 0 };
        memory.structure = FixedAllocator::create(size, sizeof(T));
        return memory;
    }

    template <typename T = USZ>
    inline static Block<T, ExpandingAllocator> dynamic(USZ size) {
        Block<T, ExpandingAllocator> memory = { 0 };
        memory.structure = ExpandingAllocator::create(size, sizeof(T));
        return memory;
    }

    template <typename T = USZ, typename A>
    inline static T* alloc(Block<T, A>* block, T v) {
        return (T*) A::insert(block->structure, (U8*) &v);
    }

    template <typename T = USZ, typename A>
    inline static T* alloc(Block<T, A>* block, T* v) {
        return (T*) A::insert(block->structure, (U8*) v);
    }

    template <typename T = USZ, typename A>
    inline static void dealloc(Block<T, A>* block, T** ref) {
        A::remove(block->structure, (U8**) ref);
    }

    template <typename T = USZ, typename A>
    inline static void dealloc(Block<T, A>* block) {
        A::destroy(block->structure);
        *block = { 0 };
    }

    /*
        base allocation calls
    */

    template <typename T = void>
    inline static T* alloc(USZ size) { 
        T* ret = (T*) malloc(size);
        return ret;
    }
    
    template <typename T = void>
    inline static T* alloc(USZ count, USZ size) { 
        T* ret = (T*) calloc(count, size);
        return (T*) calloc(count, size); 
    }

    template <typename T = void>
    inline static void dealloc(T* ptr) {
        free(ptr); 
    }

    template <typename T>
    static T* resize(T* ptr, USZ size) {
        return (T*) realloc(ptr, size);
    }

    template <typename T = void>
    inline static T* type_alloc(USZ padding = 0) {
        return alloc<T>(sizeof(T) + padding);
    }

    template <typename B = void, typename A = void>
    inline static void copy(B* dst, A* ptr, USZ size) {
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
    inline static void set(T* dst, int val, USZ size) {
        memset(dst, val, size);
    }

    template <typename T = void>
    inline static USZ compare(T* a, T* b, USZ size) {
        return (USZ) memcmp(a, b, size);
    }
};

/*
    data
*/

struct FixedArray {
    const USZ element_size; // size of each element in the array
    const USZ elements;     // number of elements in the array
    const USZ size_bytes;   // total size in bytes
    U8 data[];

    // create a fixed array with number of elements and size of each elemet
    static FixedArray* create(USZ elements, USZ element_size);

    // create a fixed array from a c style buffer
    static FixedArray* create(const U8* buffer, USZ elements, USZ element_size);

    // destroy a fixed array
    static void destroy(FixedArray* array);

    // returns pointer to data
    static U8* begin(FixedArray* array);

    // returns pointer to the end of data
    static U8* end(FixedArray* array);

    // returns pointer at an index
    static U8* at(FixedArray* array, USZ idx);

    // inserts data at an index
    static void insert(FixedArray* array, USZ idx, U8* data);

    // copys one fixed arrays data into another
    static void copy(FixedArray* a, FixedArray* b);

    // checks if two fixed arrays are identical
    static bool equals(FixedArray* a, FixedArray* b);

    // returns the size of the array in elements
    static USZ size(FixedArray* array);

    // memsets array to 0
    static void clear(FixedArray* array);
};

struct ExpandingArray {
    const USZ reserved;        // number of slots per allocation
    const USZ element_size;    // bytes per slot
    USZ max_elements;          // max number of slots with current allocation
    USZ elements_used;         // total slots used
    U8* data;

    // creates an expanding array with the size of each element
    static ExpandingArray* create(USZ element_size, USZ reserved);

    // creates an expanding array from a c style buffer
    static ExpandingArray* create(const U8* buffer, USZ element_size, USZ num_elements);

    // destroys an expanding array
    static void destroy(ExpandingArray* array);

    // doubles the array capacity
    static void expand(ExpandingArray* array);

    // expands the array capacity by an ammount
    static void expand(ExpandingArray* array, USZ amount); 

    // shrinks to exact size in bytes
    static void shrink(ExpandingArray* array);

    // concats raw bytes onto the array
    static void join_raw(ExpandingArray* array, const U8* raw, USZ size);

    // pushes value to the back of the array
    static void push_back(ExpandingArray* array, U8* val);

    // pops value off of the back of the array
    static void pop_back(ExpandingArray* array);

    // unsafe get method at an index
    static U8* get(ExpandingArray* array, USZ idx);

    // replaces value at an index
    static void set(ExpandingArray* array, USZ idx, U8* val);

    // safe get method at an index
    static U8* at(ExpandingArray* array, USZ idx);

    // inserts value at an index and offsets the array
    static void insert(ExpandingArray* array, USZ idx, U8* val);

    // erases value at an index and offsets the array
    static void erase(ExpandingArray* array, USZ idx);

    // returns raw data pointer to the beginning of the array
    static U8* begin(ExpandingArray* array);

    // returns raw pointer to the end of the array
    static U8* end(ExpandingArray* array);

    // returns total elements in the array
    static USZ size(ExpandingArray* array);

    // returns the total allocated elements in the array
    static USZ capacity(ExpandingArray* array);

    // returns if the array is emty
    static USZ empty(ExpandingArray* array);

    // clears the array (this also shrinks it down to 1 byte...)
    static void clear(ExpandingArray* array);

    // checks if this array is identical to another one
    static bool equals(ExpandingArray* a, ExpandingArray* b);
};

struct Data {

    #ifndef EXPANDING_DATA_RESERVE
        // default reserved value for an expanding array
        static constexpr USZ EXPANDING_DATA_RESERVE = 16;
    #endif

    /*
        fixed array
    */

    template <typename T>
    inline static Block<T, FixedArray> list(USZ size) {
        return Block<T, FixedArray> { .structure =  FixedArray::create(size, sizeof(T)) };
    }

    template <typename T>
    inline static Block<T, FixedArray> list(T* data, USZ len) {
        return Block<T, FixedArray> { .structure = FixedArray::create(data, len, sizeof(T)) };
    }

    template <typename T>
    inline static Block<T, FixedArray> list(Block<T, ExpandingArray> expanding) {
        return Block<T, FixedArray> { .structure = FixedArray::create((U8*) raw(expanding), size(expanding), sizeof(T)) };
    }

    template <typename T>
    inline static Block<T, FixedArray> list(std::initializer_list<T> data) {
        FixedArray* array = FixedArray::create(data.size(), sizeof(T));

        USZ idx = 0;
        for (auto it = data.begin(); it != data.end(); ++it) {
            T v = *it;
            FixedArray::insert(array, idx, (U8*) &v);
            ++idx;
        }

        return Block<T, FixedArray> { .structure = array };
    }

    template <typename T>
    inline static void insert(Block<T, FixedArray> array, USZ idx, T data) {
        FixedArray::insert(array.structure, idx, (U8*) &data);
    }

    template <typename T>
    inline static void insert(Block<T, FixedArray> array, USZ idx, T* data) {
        FixedArray::insert(array.structure, idx, (U8*) data);
    }

    template <typename T>
    inline static T* at(Block<T, FixedArray> array, int idx) {
        return (T*) FixedArray::at(array.structure, idx);
    }

    template <typename T>
    inline static USZ size(Block<T, FixedArray> array) {
        return FixedArray::size(array.structure);
    }

    template <typename T>
    inline static void clear(Block<T, FixedArray> array) {
        FixedArray::clear(array.structure);
    }

    template <typename T>
    inline static T* raw(Block<T, FixedArray> array) {
        return (T*) array.structure->data;
    }

    template <typename T>
    inline static void emplace(Block<T, FixedArray> a, Block<T, FixedArray> b) {
        FixedArray::copy(a.structure, b.structure);
    }

    template <typename T>
    inline static bool compare(Block<T, FixedArray> a, Block<T, FixedArray> b) {
        return FixedArray::equals(a.structure, b.structure);
    }

    /*
        expanding array
    */

    template <typename T>
    inline static Block<T, ExpandingArray> vector(USZ reserved = EXPANDING_DATA_RESERVE) {
        return Block<T, ExpandingArray> { .structure =  ExpandingArray::create(sizeof(T), reserved) };
    }

    template <typename T>
    inline static Block<T, ExpandingArray> vector(T* data, USZ len) {
        return Block<T, ExpandingArray> { .structure = FixedArray::create(data, len, sizeof(T)) };
    }

    template <typename T>
    inline static Block<T, ExpandingArray> vector(Block<T, FixedArray> fixed) {
        return Block<T, ExpandingArray> { .structure =  ExpandingArray::create((U8*) raw(fixed), sizeof(T), size(fixed)) };
    }

    template <typename T>
    inline static Block<T, ExpandingArray> vector(std::initializer_list<T> data) {
        ExpandingArray* array = ExpandingArray::create(sizeof(T), data.size());

        USZ idx = 0;
        for (auto it = data.begin(); it != data.end(); ++it) {
            T v = *it;
            ExpandingArray::push_back(array, (U8*) &v);
            ++idx;
        }

        return Block<T, ExpandingArray> { .structure = array };
    }

    template <typename T>
    inline static USZ size(Block<T, ExpandingArray> array) {
        return ExpandingArray::size(array.structure);
    }

    template <typename T>
    inline static USZ capacity(Block<T, ExpandingArray> array) {
        return ExpandingArray::capacity(array.structure);
    }

    template <typename T>
    inline static T* at(Block<T, ExpandingArray> array, int idx) {
        return (T*) ExpandingArray::at(array.structure, idx);
    }

    template <typename T>
    inline static T* begin(Block<T, ExpandingArray> array) {
        return (T*) ExpandingArray::begin(array.structure);
    }

    template <typename T>
    inline static T* end(Block<T, ExpandingArray> array) {
        return (T*) ExpandingArray::end(array.structure);
    }

    template <typename T>
    inline static void push_back(Block<T, ExpandingArray> array, T data) {
        ExpandingArray::push_back(array.structure, (U8*) &data);
    }

    template <typename T>
    inline static void push_back(Block<T, ExpandingArray> array, T* data) {
        ExpandingArray::push_back(array.structure, (U8*) data);
    }

    template <typename T>
    inline static void pop_back(Block<T, ExpandingArray> array) {
        ExpandingArray::pop_back(array.structure);
    }

    template <typename T>
    inline static void insert(Block<T, ExpandingArray> array, T data, USZ idx) {
        ExpandingArray::insert(array.structure, idx, (U8*) &data);
    }

    template <typename T>
    inline static void insert(Block<T, ExpandingArray> array, T* data, USZ idx) {
        ExpandingArray::insert(array.structure, idx, (U8*) data);
    }

    template <typename T>
    inline static void replace(Block<T, ExpandingArray> array, T data, USZ idx) {
        ExpandingArray::set(array.structure, idx, (U8*) &data);
    }

    template <typename T>
    inline static void replace(Block<T, ExpandingArray> array, T* data, USZ idx) {
        ExpandingArray::set(array.structure, idx, (U8*) data);
    }

    template <typename T>
    inline static void erase(Block<T, ExpandingArray> array, USZ idx) {
        ExpandingArray::erase(array.structure, idx);
    }

    template <typename T>
    inline static void clear(Block<T, ExpandingArray> array) {
        ExpandingArray::clear(array.structure);
    }

    template <typename T>
    inline static void shrink(Block<T, ExpandingArray> array) {
        ExpandingArray::shrink(array.structure);
    }

    template <typename T>
    inline static bool empty(Block<T, ExpandingArray> array) {
        return ExpandingArray::empty(array.structure);
    }

    template <typename T>
    inline static T* raw(Block<T, ExpandingArray> array) {
        return (T*) array.structure->data;
    }

    /*
        comparitors
    */

    template <typename T>
    inline static bool compare(Block<T, ExpandingArray> a, Block<T, ExpandingArray> b) {
        return ExpandingArray::equals(a.structure, b.structure);
    }

    template <typename T>
    inline static bool compare(Block<T, ExpandingArray> a, Block<T, FixedArray> b) {
        return (size(a) == size(b)) && Mem::compare(raw(a), raw(b), size(a) * sizeof(T)) == 0;
    }

    template <typename T>
    inline static bool compare(Block<T, FixedArray> a, Block<T, ExpandingArray> b) {
        return (size(a) == size(b)) && Mem::compare(raw(a), raw(b), size(a) * sizeof(T)) == 0;
    }

    /*
        strings
    */
    inline static void join(Block<U8, ExpandingArray> arr1, const U8* data) {
        ExpandingArray::join_raw(arr1.structure, data, strlen((char*) data));
    }

    inline static void join(Block<U8, ExpandingArray> arr1, const U8* data, USZ len) {
        ExpandingArray::join_raw(arr1.structure, data, len);
    }
};

template <typename T, typename A = FixedAllocator> using FixedBlock = Block<T, A>;
template <typename T, typename A = ExpandingAllocator> using DynamicBlock = Block<T, A>;
template <typename T> using List = Block<T, FixedArray>;
template <typename T> using Vector = Block<T, ExpandingArray>;
using String = Block<U8, ExpandingArray>;

}

#endif