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

using namespace nstd;

void do_something(List<char> d) {
    printf("%s\n", Data::raw(d));
}

void test2(Vector<char> d) {
    printf("test2\n");
}

void test() {
    
    /*
        basically smart pointers
    */

    FixedBlock<int>::Temp fixed_mem_auto = Mem::fixed<int>(100); // automatically freed
    DynamicBlock<int>::Temp dynamic_mem_auto = Mem::dynamic<int>(100); // automatically freed
    Vector<int>::Temp vector_auto = Data::vector<int>(); // automatically freed
    List<int>::Temp list_auto = Data::list<int>(100); // automatically freed
    
    /*
        manual allocation
    */

    FixedBlock<int> fixed_mem_manual = Mem::fixed<int>(100);
    DynamicBlock<int> dynamic_mem_manual = Mem::dynamic<int>(100);
    Vector<int> vector_manual = Data::vector<int>();
    List<int> list_manual = Data::list<int>(100);

    Mem::dealloc(&fixed_mem_manual);
    Mem::dealloc(&dynamic_mem_manual);
    Mem::dealloc(&vector_manual);
    Mem::dealloc(&list_manual);
}

int main() {
    test();

    // String my_str = Data::string("hello world");
    // Data::join(my_str, "test");

    // Array my_array = Data::list(my_str);

    // printf("%s123\n", Data::raw(my_str));
    // printf("%s123\n", Data::raw(my_array));

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