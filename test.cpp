#include "nstd.hpp"

struct Node2D {
    Node2D* next;
    Node2D* prev;
    char* reference;

    static Node2D create(char* reference) {
        Node2D node = { 0 };
        node.next = nullptr;
        node.prev = nullptr;
        node.reference = reference;
        return node;
    }

    // unlinks node from children destroying its reference
    static void destroy(Node2D* node) {
        if (!node) return;

        node->prev->next = node->next;
        node->next->prev = node->prev;
        *node = { 0 };
    }

    // finds first node in list, if the node is the first node it returns node
    static Node2D* find_begin(Node2D* node) {
        Node2D* cur = node;

        while (cur && cur->prev) {
            cur = cur->prev;
        }

        return cur;
    }

    // finds last node in list, if the node is the last node it returns node
    static Node2D* find_end(Node2D* node) {
        Node2D* cur = node;

        while (cur && cur->next) {
            cur = cur->next;
        }

        return cur;
    }

    // links a to the front of b
    static void link_front(Node2D* a, Node2D* b) {
        if (!a || !b) return;

        Node2D* a_begin = find_begin(a);
        Node2D* b_end = find_end(b);

        a_begin->prev = b_end;
        b_end->next = a_begin;
    }

    // links a to the back of b
    static void link_back(Node2D* a, Node2D* b) {
        if (!a || !b) return;

        Node2D* a_end = find_end(a);
        Node2D* b_begin = find_begin(b);

        a_end->next = b_begin;
        b_begin->prev = a_end;
    }

    // inserts a in front of b
    static void link_front_insert(Node2D* a, Node2D* b) {
        if (!a || !b) return;

        Node2D* a_begin = find_begin(a);
        Node2D* a_end = find_end(a);
        Node2D* b_next = b->next;

        b_next->prev = a_end;
        a_end->next = b_next;

        a_begin->prev = b;
        b->next = a_begin;
    }

    // insert a behind b
    static void link_back_insert(Node2D* a, Node2D* b) {
        if (!a || !b) return;

        Node2D* a_begin = find_begin(a);
        Node2D* a_end = find_end(a);
        Node2D* b_prev = b->prev;

        b_prev->next = a_begin;
        a_begin->prev = b_prev;

        a_end->next = b;
        b->prev = a_end;
    }
};

struct FixedArray {
    const size_t element_size;
    const size_t elements;
    const size_t size_bytes;
    char data[];

    static FixedArray* create(size_t elements, size_t element_size) {
        size_t size = elements * element_size;

        FixedArray* array = Mem::type_alloc<FixedArray>(size);
        Mem::const_copy(&array->size_bytes, &size);
        Mem::const_copy(&array->elements, &elements);
        Mem::const_copy(&array->element_size, &element_size);

        Mem::set(array->data, 0, size);

        return array;
    }

    static void destroy(FixedArray* array) {
        Mem::dealloc(array);
    }

    static char* begin(FixedArray* array) {
        return array->data;
    }

    static char* end(FixedArray* array) {
        return array->data + array->size_bytes;
    }

    static bool contains(FixedArray* array, const char* ptr) {
        return ptr >= begin(array) && ptr < end(array);
    }

    static bool is_aligned(FixedArray* array, char* ptr) {
        return contains(array, ptr) && (
               (ptr - array->data) % array->element_size) == 0;
    }

    static char* next(FixedArray* array, char* ptr) {
        if (!contains(array, ptr)) {
            return nullptr;
        }

        return ptr + array->element_size;
    }

    static void next(FixedArray* array, char** ptr) {
        if (!contains(array, *ptr)) {
            *ptr = nullptr;
            return;
        }

        *ptr = *ptr + array->element_size;
    }

    static char* at(FixedArray* array, size_t idx) {
        if (idx > array->elements) {
            return nullptr;
        }

        return array->data + idx * array->element_size;
    }

    static void iterate(FixedArray* array, void (*onitem)(char* ptr, size_t idx)) {
        size_t i = 0;
        for (
            char* ref = begin(array);
            contains(array, ref);
            FixedArray::next(array, &ref), ++i
        ) {
            onitem(ref, i);
        }
    }
};

struct ExpandingArray {
    const size_t chunk_elements;  // number of slots per allocation
    const size_t element_size;    // bytes per slot
    size_t max_elements;          // max number of slots with current allocation
    size_t elements_used;         // total slots used
    char* data;

    static ExpandingArray* create(size_t element_size, size_t reserved = 32) {
        ExpandingArray* array = Mem::type_alloc<ExpandingArray>();
        array->data = Mem::alloc<char>(element_size * reserved);
        Mem::set(array->data, 0, element_size * reserved);
        Mem::const_copy(&array->chunk_elements, &reserved);
        Mem::const_copy(&array->element_size, &element_size);
        array->max_elements = reserved;
        array->elements_used = 0;
        return array;
    }

    static void destroy(ExpandingArray* array) {
        Mem::dealloc(array->data);
        Mem::dealloc(array);
    }

    static void expand(ExpandingArray* array) {
        size_t new_size = array->max_elements * 2;
        array->data = Mem::resize(array->data, new_size * array->element_size);
        array->max_elements = new_size;
    }

    static void shrink(ExpandingArray* array) {
        size_t chunk_boundry = (array->elements_used / array->chunk_elements) + 1;
        size_t new_size = chunk_boundry * array->chunk_elements;

        if (new_size < array->chunk_elements) {
            new_size = array->chunk_elements;
        }

        if (new_size == array->max_elements) {
            return;
        }

        array->data = Mem::resize(array->data, new_size * array->element_size);
        array->max_elements = new_size;
    }

    static void push_back(ExpandingArray* array, char* val) {
        if (array->elements_used - array->max_elements == 0) {
            expand(array);
        }

        memcpy(array->data + array->elements_used * array->element_size, val, array->element_size);
        array->elements_used++;
    }

    static void pop_back(ExpandingArray* array) {
        array->elements_used--;
    }

    static char* back(ExpandingArray* array) {
        if (array->elements_used == 0) {
            return nullptr;
        }

        return array->data + (array->elements_used - 1) * array->element_size;
    }

    static char* get(ExpandingArray* array, size_t idx) {
        return array->data + idx * array->element_size;
    }

    static void set(ExpandingArray* array, size_t idx, char* val) {
        if (idx >= array->elements_used) {
            THROW("index out of bounds");
        }

        Mem::copy(get(array, idx), val, array->element_size);
    }

    static char* at(ExpandingArray* array, size_t idx) {
        if (idx >= array->elements_used) {
            THROW("index out of bounds");
        }

        return get(array, idx);
    }

    static void insert(ExpandingArray* array, size_t idx, char* val) {
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

    static void erase(ExpandingArray* array, size_t idx) {
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

    static char* begin(ExpandingArray* array) {
        return array->data;
    }

    static char* end(ExpandingArray* array) {
        return array->data + (array->elements_used - 1) * array->element_size;
    }

    static size_t size(ExpandingArray* array) {
        return array->elements_used;
    }

    static size_t capacity(ExpandingArray* array) {
        return array->max_elements;
    }

    static size_t empty(ExpandingArray* array) {
        return array->elements_used == 0;
    }

    static void clear(ExpandingArray* array) {
        array->elements_used = 0;
        shrink(array);
    }
};

struct Test {
    int a;
    int b;
    int c;
    size_t d;
};

#include <chrono>
#include <vector>

int main() {
    // MyObject* obj = MyObject::create();
    // MyObject::destroy(obj);

//     ExpandingArray* array = ExpandingArray::create(sizeof(Test));

//     for (int i = 0; i < 10; i++) {
//         Test v = {10 + i, 20 + i, 30 + i, 40 + i};
//         ExpandingArray::push_back(array, (char*) &v);
//     }

//     for (int i = 0; i < array->elements_used; i++) {
//         Test v = *(Test*) ExpandingArray::at(array, i);
//         printf("%i %i %i %i\n", v.a, v.b, v.c, v.d);
//     }

//     printf("\n\n");

//     Test v = { 0 };

//     ExpandingArray::insert(array, 3, (char*) &v);
//     ExpandingArray::insert(array, 4, (char*) &v);
//     ExpandingArray::insert(array, 5, (char*) &v);

//     for (int i = 0; i < array->elements_used; i++) {
//         Test v = *(Test*) ExpandingArray::at(array, i);
//         printf("%i %i %i %i\n", v.a, v.b, v.c, v.d);
//     }

//     printf("\n\n");

//     ExpandingArray::erase(array, 6);

//     for (int i = 0; i < array->elements_used; i++) {
//         Test v = *(Test*) ExpandingArray::at(array, i);
//         printf("%i %i %i %i\n", v.a, v.b, v.c, v.d);
//     }

//     // size_t insertions = 1000000;
//     // {
//     //     ExpandingArray* arr1 = ExpandingArray::create(sizeof(Test));

//     //     auto start_insert = std::chrono::high_resolution_clock::now();

//     //     for (int i = 0; i < insertions; i++) {
//     //         Test val = {
//     //             i, i, i, i, nullptr
//     //         };

//     //         ExpandingArray::push_back(arr1, (char*) &val);
//     //     }

//     //     ExpandingArray::shrink(arr1);

//     //     auto end_insert = std::chrono::high_resolution_clock::now();
//     //     double insertion_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_insert - start_insert).count();

//     //     auto start_pop = std::chrono::high_resolution_clock::now();

//     //     for (int i = 0; i < insertions; i++) {
//     //         Test v = *(Test*) ExpandingArray::back(arr1);
//     //         ExpandingArray::pop_back(arr1);
//     //     }

//     //     auto end_pop = std::chrono::high_resolution_clock::now();
//     //     double pop_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_pop - start_pop).count();

//     //     ExpandingArray::destroy(arr1);

//     //     printf("%.8f, %.8f\n", insertion_time, pop_time);
//     // }

    return 0;
}