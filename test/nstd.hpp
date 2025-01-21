#ifndef _NSTD_HPP
#define _NSTD_HPP

#include <iostream>
#include <cstring>
#include <stdint.h>

#ifdef STD_HEAP_TRACK
#   include <map>
#endif

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

inline USZ min(USZ a, USZ b) { return (a < b) ? a : b; };
inline USZ max(USZ a, USZ b) { return (a < b) ? b : a; };

/*
    memory
*/

// fixed size block of memory
struct FixedHeapContainer {

#   ifndef AUTO_ASSERT_MEMORY_ALIGNED
       // enables auto alignment assertion (~5-10% speed gain, but removes safety)
       static constexpr bool AUTO_ASSERT_MEMORY_ALIGNED = false;
#   endif

    FixedHeapContainer* next;
    USZ chunks_free;
    const USZ chunk_count;
    const USZ chunk_size;
    U8** free_list;
    U8 allocated[];

    // creates a new block of memory
    static FixedHeapContainer* create(USZ chunk_count, USZ chunk_size);

    // destroys a mem block and all linked blocks
    static void destroy(FixedHeapContainer* block);

    // inserts a slice of data if it can
    static U8* insert(FixedHeapContainer* block, U8* data);

    // removes a slice of data by reference
    static void remove(FixedHeapContainer* block, U8** data, bool assert_aligned = AUTO_ASSERT_MEMORY_ALIGNED);

    // returns whether an address is aligned with the memory block
    static bool check_addr_alignment(FixedHeapContainer* block, U8* addr);

    // check if fixed allocator has space
    static bool has_space(FixedHeapContainer* block);
};

// dynamic array of memory blocks
struct DynamicHeapContainer {
    FixedHeapContainer* root;
    FixedHeapContainer* last;
    FixedHeapContainer* last_empty;
    FixedHeapContainer* last_removed;

    // create a new expanding allocator, this is like a fixed allocator but with no maximum size
    static DynamicHeapContainer* create(USZ chunk_count, USZ chunk_size);

    // destroy an expanding allocator
    static void destroy(DynamicHeapContainer* pool);

    // conactinates a new pool onto the allocator
    static void expand(DynamicHeapContainer* pool);

    // insert into the allocator
    static U8* insert(DynamicHeapContainer* pool, U8* data);

    // remove from the allocator
    static void remove(DynamicHeapContainer* pool, U8** data);

    // returns a free block in the allocator
    static FixedHeapContainer* find_free_block(DynamicHeapContainer* pool);

    // finds the block containing a reference
    static FixedHeapContainer* find_aligned_block(DynamicHeapContainer* pool, U8* reference);
};

/*
    data
*/

struct Hash {
    struct Group {
        void (*func)(U32*, const U32);
        U32 seed;
    };

    static constexpr Group djb2 {
        .func = [](U32* hash, const U32 v) -> void {
            *hash = ((*hash << 5) + *hash) + v;
        },
        .seed = 5381
    };

    static constexpr Group fnv1a {
        .func = [](U32* hash, const U32 v) -> void {
            *hash ^= v;
            *hash *= 16777619u;
        },
        .seed = 2166136261u
    };

    // generates a hash with a defined hash function
    static U32 generate(const U8* bytes, USZ len, const Group group);
};

struct Pair {
    U8* lptr;
    U8* rptr;

    // compare lvalue and rvalue
    static USZ compare_lr(const Pair* a, const Pair* b);

    // compare only l value
    static USZ compare_l(const Pair* a, const Pair* b);

    // compare only r value
    static USZ compare_r(const Pair* a, const Pair* b);
};

struct ReferenceTable {
    struct Item {
        Pair region;
        Item* next;
    };

    Hash::Group hash_type;
    DynamicBlock<Item> allocator; 
    List<Item*> references;

    // creates a new reference table, this makes some allocations
    static ReferenceTable* create(USZ width, Hash::Group hash_type = Hash::djb2);

    // frees up all rescources in the reference table
    static void destroy(ReferenceTable* table);

    // gets an item by an arbitrary ptr
    static Item* get_item(ReferenceTable* table, const U8* ptr);

    // inserts a pair of items
    static void insert(ReferenceTable* table, Pair data);

    // finds an item by an arbitrary ptr
    static U8* find(ReferenceTable* table, const U8* lptr);

    // iterates over all key value pairs and calls it for each one
    static void iterate(ReferenceTable* table, void (*it)(U8*, U8*));
};

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

/*
    generic interface
*/

template <typename T, typename A>
struct Block {
    A* structure = nullptr;

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

template <typename T> using FixedBlock = Block<T, FixedHeapContainer>;
template <typename T> using DynamicBlock = Block<T, DynamicHeapContainer>;
template <typename T> using List = Block<T, FixedArray>;
template <typename T> using Vector = Block<T, ExpandingArray>;
using String = Block<U8, ExpandingArray>;

// wrapper of malloc for profiling
struct Mem {

#   ifdef STD_HEAP_TRACK

    static std::map<U8*, USZ> heap_used;

    static USZ alloc_calls;
    static USZ realloc_calls;
    static USZ free_calls;

#   endif

    static USZ heap_total();

    /*
        allocate to buffer
    */

    template <typename T = USZ>
    inline static Block<T, FixedHeapContainer> fixed_container(USZ size);

    template <typename T = USZ>
    inline static Block<T, DynamicHeapContainer> dynamic_container(USZ size);

    template <typename T = USZ, typename A>
    inline static T* alloc(Block<T, A>* block);

    template <typename T = USZ, typename A>
    inline static T* alloc(Block<T, A>* block, T v);

    template <typename T = USZ, typename A>
    inline static T* alloc(Block<T, A>* block, T* v);

    /*
        primitive deallocation
    */

    template <typename T = USZ, typename A>
    inline static void dealloc(Block<T, A>* block, T** ref);

    template <typename T = USZ, typename A>
    inline static void dealloc(Block<T, A>* block);
    
    /*
        recursive deallocation
    */

    template <typename U, typename B>
    inline static void dealloc(Block<Block<U, B>, ExpandingArray>* block);

    template <typename U, typename B>
    inline static void dealloc(Block<Block<U, B>, FixedArray>* block);

    /*
        base allocation calls
    */

    template <typename T>
    inline static T* alloc(USZ size);
    
    template <typename T>
    inline static T* alloc(USZ count, USZ size);

    template <typename T>
    inline static void dealloc(T* ptr);

    template <typename T>
    static T* resize(T* ptr, USZ size);

    template <typename T>
    inline static T* type_alloc(USZ padding = 0);

    template <typename B, typename A>
    inline static void copy(B* dst, A* ptr, USZ size);

    template <typename B, typename A>
    inline static void copy(B* dst, A* ptr);

    template <typename B, typename A>
    inline static void const_copy(const B* dst, const A* ptr);

    template <typename T>
    inline static void set(T* dst, int val, USZ size);

    template <typename T>
    inline static void set(T* dst, int val);

    template <typename T>
    inline static USZ compare(const T* a, const T* b, USZ size);

    template <typename T>
    inline static USZ compare(const T* a, const T* b);
};

// wrapper for data structures
struct Data {

    #ifndef EXPANDING_DATA_RESERVE
        // default reserved value for an expanding array
        static constexpr USZ EXPANDING_DATA_RESERVE = 16;
    #endif

    /*
        fixed array
    */

    template <typename T>
    inline static Block<T, FixedArray> list(USZ size);

    template <typename T>
    inline static Block<T, FixedArray> list(T* data, USZ len);

    template <typename T>
    inline static Block<T, FixedArray> list(Block<T, ExpandingArray> expanding);

    template <typename T>
    inline static Block<T, FixedArray> list(std::initializer_list<T> data);

    template <typename T>
    inline static void insert(Block<T, FixedArray> array, USZ idx, T data);

    template <typename T>
    inline static void insert(Block<T, FixedArray> array, USZ idx, T* data);

    template <typename T>
    inline static T* at(Block<T, FixedArray> array, int idx);

    template <typename T>
    inline static T* at(Block<T, FixedArray>* array, int idx);

    template <typename T>
    inline static USZ size(Block<T, FixedArray> array);

    template <typename T>
    inline static void clear(Block<T, FixedArray> array);

    template <typename T>
    inline static T* raw(Block<T, FixedArray> array);

    template <typename T>
    inline static void emplace(Block<T, FixedArray> a, Block<T, FixedArray> b);

    template <typename T>
    inline static bool compare(Block<T, FixedArray> a, Block<T, FixedArray> b);

    /*
        expanding array
    */

    template <typename T>
    inline static Block<T, ExpandingArray> vector(USZ reserved = EXPANDING_DATA_RESERVE);

    template <typename T>
    inline static Block<T, ExpandingArray> vector(T* data, USZ len);

    template <typename T>
    inline static Block<T, ExpandingArray> vector(Block<T, FixedArray> fixed);

    template <typename T>
    inline static Block<T, ExpandingArray> vector(Block<T, ExpandingArray> clone);

    template <typename T>
    inline static Block<T, ExpandingArray> vector(std::initializer_list<T> data);

    template <typename T>
    inline static USZ size(Block<T, ExpandingArray> array);

    template <typename T>
    inline static USZ capacity(Block<T, ExpandingArray> array);

    template <typename T>
    inline static T* at(Block<T, ExpandingArray> array, int idx);

    template <typename T>
    inline static T* at(Block<T, ExpandingArray>* array, int idx);

    template <typename T>
    inline static T* begin(Block<T, ExpandingArray> array);

    template <typename T>
    inline static T* end(Block<T, ExpandingArray> array);

    template <typename T>
    inline static void push_back(Block<T, ExpandingArray> array, T data);

    template <typename T>
    inline static void push_back(Block<T, ExpandingArray> array, T* data);

    template <typename T>
    inline static void pop_back(Block<T, ExpandingArray> array);

    template <typename T>
    inline static void insert(Block<T, ExpandingArray> array, T data, USZ idx);

    template <typename T>
    inline static void insert(Block<T, ExpandingArray> array, T* data, USZ idx);

    template <typename T>
    inline static void replace(Block<T, ExpandingArray> array, T data, USZ idx);

    template <typename T>
    inline static void replace(Block<T, ExpandingArray> array, T* data, USZ idx);

    template <typename T>
    inline static void erase(Block<T, ExpandingArray> array, USZ idx);

    template <typename T>
    inline static void clear(Block<T, ExpandingArray> array);

    template <typename T>
    inline static void shrink(Block<T, ExpandingArray> array);

    template <typename T>
    inline static bool empty(Block<T, ExpandingArray> array);

    template <typename T>
    inline static T* raw(Block<T, ExpandingArray> array);

    /*
        comparitors
    */

    template <typename T>
    inline static bool compare(Block<T, ExpandingArray> a, Block<T, ExpandingArray> b);

    template <typename T>
    inline static bool compare(Block<T, ExpandingArray> a, Block<T, FixedArray> b);

    template <typename T>
    inline static bool compare(Block<T, FixedArray> a, Block<T, ExpandingArray> b);

    /*
        strings
    */
    inline static void join(Block<U8, ExpandingArray> arr1, const U8* data);

    inline static void join(Block<U8, ExpandingArray> arr1, const U8* data, USZ len);
};

/*
    allocate to buffer
*/

template <typename T>
inline Block<T, FixedHeapContainer> Mem::fixed_container(USZ size) {
    Block<T, FixedHeapContainer> memory = { 0 };
    memory.structure = FixedHeapContainer::create(size, sizeof(T));
    return memory;
}

template <typename T>
inline Block<T, DynamicHeapContainer> Mem::dynamic_container(USZ size) {
    Block<T, DynamicHeapContainer> memory = { 0 };
    memory.structure = DynamicHeapContainer::create(size, sizeof(T));
    return memory;
}

template <typename T, typename A>
inline T* Mem::alloc(Block<T, A>* block) {
    return (T*) A::insert(block->structure, nullptr);
}

template <typename T, typename A>
inline T* Mem::alloc(Block<T, A>* block, T v) {
    return (T*) A::insert(block->structure, (U8*) &v);
}

template <typename T, typename A>
inline T* Mem::alloc(Block<T, A>* block, T* v) {
    return (T*) A::insert(block->structure, (U8*) v);
}

/*
    primitive deallocation
*/

template <typename T, typename A>
inline void Mem::dealloc(Block<T, A>* block, T** ref) {
    A::remove(block->structure, (U8**) ref);
}

template <typename T, typename A>
inline void Mem::dealloc(Block<T, A>* block) {
    A::destroy(block->structure);
    *block = { 0 };
}

/*
    recursive deallocation
*/

template <typename U, typename B>
inline void Mem::dealloc(Block<Block<U, B>, ExpandingArray>* block) {
    for (USZ i = 0; i < Data::size(*block); i++) {
        Block<U, B>* blk = Data::at(*block, i);
        Mem::dealloc(blk);
    }
    ExpandingArray::destroy(block->structure);
    *block = { 0 };
}

template <typename U, typename B>
inline void Mem::dealloc(Block<Block<U, B>, FixedArray>* block) {
    for (USZ i = 0; i < Data::size(*block); i++) {
        Block<U, B>* blk = Data::at(*block, i);
        Mem::dealloc(blk);
    }
    FixedArray::destroy(block->structure);
    *block = { 0 };
}

/*
    base allocation calls
*/

template <typename T = void>
inline T* Mem::alloc(USZ size) { 
    T* ret = (T*) malloc(size);

#   ifdef STD_HEAP_TRACK

    heap_used.insert({(U8*) ret, size});
    alloc_calls++;

#   endif

    return ret;
}

template <typename T = void>
inline T* Mem::alloc(USZ count, USZ size) { 
    T* ret = (T*) calloc(count, size);

#   ifdef STD_HEAP_TRACK

    heap_used.insert({(U8*) ret, size * count});
    alloc_calls++;

#   endif

    return (T*) calloc(count, size); 
}

template <typename T>
inline void Mem::dealloc(T* ptr) {
#   ifdef STD_HEAP_TRACK

    if (heap_used.count((U8*) ptr)) {
        heap_used.erase((U8*) ptr);
    }

    free_calls++;

#   endif

    free(ptr); 
}

template <typename T>
inline T* Mem::resize(T* ptr, USZ size) {
    T* nptr = (T*) realloc(ptr, size);

#   ifdef STD_HEAP_TRACK

    if (heap_used.count((U8*) ptr)) {
        heap_used.erase(ptr);
        heap_used.insert({nptr, size});
    }

    realloc_calls++;

#   endif

    return nptr;
}

/*
    movement and special allocation
*/

template <typename T>
inline T* Mem::type_alloc(USZ padding) {
    return alloc<T>(sizeof(T) + padding);
}

template <typename B, typename A>
inline void Mem::copy(B* dst, A* ptr, USZ size) {
    memcpy(dst, ptr, size);
}

template <typename B, typename A>
inline void Mem::copy(B* dst, A* ptr) {
    memcpy(dst, ptr, sizeof(A));
}

template <typename B, typename A>
inline void Mem::const_copy(const B* dst, const A* ptr) {
    memcpy((B*) dst, (A*) ptr, sizeof(A));
}

template <typename T>
inline void Mem::set(T* dst, int val, USZ size) {
    memset(dst, val, size);
}

template <typename T>
inline void Mem::set(T* dst, int val) {
    memset(dst, val, sizeof(T));
}

template <typename T>
inline USZ Mem::compare(const T* a, const T* b, USZ size) {
    return (USZ) memcmp(a, b, size);
}

template <typename T>
inline USZ Mem::compare(const T* a, const T* b) {
    return (USZ) memcmp(a, b, sizeof(T));
}

/*
    fixed array
*/

template <typename T>
inline Block<T, FixedArray> Data::list(USZ size) {
    return Block<T, FixedArray> { .structure =  FixedArray::create(size, sizeof(T)) };
}

template <typename T>
inline Block<T, FixedArray> Data::list(T* data, USZ len) {
    return Block<T, FixedArray> { .structure = FixedArray::create(data, len, sizeof(T)) };
}

template <typename T>
inline Block<T, FixedArray> Data::list(Block<T, ExpandingArray> expanding) {
    return Block<T, FixedArray> { .structure = FixedArray::create((U8*) raw(expanding), size(expanding), sizeof(T)) };
}

template <typename T>
inline Block<T, FixedArray> Data::list(std::initializer_list<T> data) {
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
inline void Data::insert(Block<T, FixedArray> array, USZ idx, T data) {
    FixedArray::insert(array.structure, idx, (U8*) &data);
}

template <typename T>
inline void Data::insert(Block<T, FixedArray> array, USZ idx, T* data) {
    FixedArray::insert(array.structure, idx, (U8*) data);
}

template <typename T>
inline T* Data::at(Block<T, FixedArray> array, int idx) {
    return (T*) FixedArray::at(array.structure, idx);
}

template <typename T>
inline T* Data::at(Block<T, FixedArray>* array, int idx) {
    return (T*) FixedArray::at(array->structure, idx);
}

template <typename T>
inline USZ Data::size(Block<T, FixedArray> array) {
    return FixedArray::size(array.structure);
}

template <typename T>
inline void Data::clear(Block<T, FixedArray> array) {
    FixedArray::clear(array.structure);
}

template <typename T>
inline T* Data::raw(Block<T, FixedArray> array) {
    return (T*) array.structure->data;
}

template <typename T>
inline void Data::emplace(Block<T, FixedArray> a, Block<T, FixedArray> b) {
    FixedArray::copy(a.structure, b.structure);
}

template <typename T>
inline bool Data::compare(Block<T, FixedArray> a, Block<T, FixedArray> b) {
    return FixedArray::equals(a.structure, b.structure);
}

/*
    expanding array
*/

template <typename T>
inline Block<T, ExpandingArray> Data::vector(USZ reserved) {
    return Block<T, ExpandingArray> { .structure =  ExpandingArray::create(sizeof(T), reserved) };
}

template <typename T>
inline Block<T, ExpandingArray> Data::vector(T* data, USZ len) {
    return Block<T, ExpandingArray> { .structure = FixedArray::create(data, len, sizeof(T)) };
}

template <typename T>
inline Block<T, ExpandingArray> Data::vector(Block<T, FixedArray> fixed) {
    return Block<T, ExpandingArray> { .structure =  ExpandingArray::create((U8*) raw(fixed), sizeof(T), size(fixed)) };
}

template <typename T>
inline Block<T, ExpandingArray> Data::vector(Block<T, ExpandingArray> clone) {
    return Block<T, ExpandingArray> { .structure =  ExpandingArray::create((U8*) raw(clone), sizeof(T), size(clone)) };
}

template <typename T>
inline Block<T, ExpandingArray> Data::vector(std::initializer_list<T> data) {
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
inline USZ Data::size(Block<T, ExpandingArray> array) {
    return ExpandingArray::size(array.structure);
}

template <typename T>
inline USZ Data::capacity(Block<T, ExpandingArray> array) {
    return ExpandingArray::capacity(array.structure);
}

template <typename T>
inline T* Data::at(Block<T, ExpandingArray> array, int idx) {
    return (T*) ExpandingArray::at(array.structure, idx);
}

template <typename T>
inline T* Data::at(Block<T, ExpandingArray>* array, int idx) {
    return (T*) ExpandingArray::at(array->structure, idx);
}

template <typename T>
inline T* Data::begin(Block<T, ExpandingArray> array) {
    return (T*) ExpandingArray::begin(array.structure);
}

template <typename T>
inline T* Data::end(Block<T, ExpandingArray> array) {
    return (T*) ExpandingArray::end(array.structure);
}

template <typename T>
inline void Data::push_back(Block<T, ExpandingArray> array, T data) {
    ExpandingArray::push_back(array.structure, (U8*) &data);
}

template <typename T>
inline void push_back(Block<T, ExpandingArray> array, T* data) {
    ExpandingArray::push_back(array.structure, (U8*) data);
}

template <typename T>
inline void Data::pop_back(Block<T, ExpandingArray> array) {
    ExpandingArray::pop_back(array.structure);
}

template <typename T>
inline void Data::insert(Block<T, ExpandingArray> array, T data, USZ idx) {
    ExpandingArray::insert(array.structure, idx, (U8*) &data);
}

template <typename T>
inline void Data::insert(Block<T, ExpandingArray> array, T* data, USZ idx) {
    ExpandingArray::insert(array.structure, idx, (U8*) data);
}

template <typename T>
inline void Data::replace(Block<T, ExpandingArray> array, T data, USZ idx) {
    ExpandingArray::set(array.structure, idx, (U8*) &data);
}

template <typename T>
inline void Data::replace(Block<T, ExpandingArray> array, T* data, USZ idx) {
    ExpandingArray::set(array.structure, idx, (U8*) data);
}

template <typename T>
inline void Data::erase(Block<T, ExpandingArray> array, USZ idx) {
    ExpandingArray::erase(array.structure, idx);
}

template <typename T>
inline void Data::clear(Block<T, ExpandingArray> array) {
    ExpandingArray::clear(array.structure);
}

template <typename T>
inline void Data::shrink(Block<T, ExpandingArray> array) {
    ExpandingArray::shrink(array.structure);
}

template <typename T>
inline bool Data::empty(Block<T, ExpandingArray> array) {
    return ExpandingArray::empty(array.structure);
}

template <typename T>
inline T* Data::raw(Block<T, ExpandingArray> array) {
    return (T*) array.structure->data;
}

/*
    comparitors
*/

template <typename T>
inline bool Data::compare(Block<T, ExpandingArray> a, Block<T, ExpandingArray> b) {
    return ExpandingArray::equals(a.structure, b.structure);
}

template <typename T>
inline bool Data::compare(Block<T, ExpandingArray> a, Block<T, FixedArray> b) {
    return (size(a) == size(b)) && Mem::compare(raw(a), raw(b), size(a) * sizeof(T)) == 0;
}

template <typename T>
inline bool Data::compare(Block<T, FixedArray> a, Block<T, ExpandingArray> b) {
    return (size(a) == size(b)) && Mem::compare(raw(a), raw(b), size(a) * sizeof(T)) == 0;
}

/*
    strings
*/
inline void Data::join(Block<U8, ExpandingArray> arr1, const U8* data) {
    ExpandingArray::join_raw(arr1.structure, data, strlen((char*) data));
}

inline void Data::join(Block<U8, ExpandingArray> arr1, const U8* data, USZ len) {
    ExpandingArray::join_raw(arr1.structure, data, len);
}

}

#endif