// #define STD_HEAP_TRACK
#include "nstd.hpp"

using namespace nstd;





struct MyStruct {
    int a;
    int b;
};

int main() {

    ReferenceTable* tab = ReferenceTable::create(100); // not in API yet so cant be temp also returns raw pointer
    FixedBlock<MyStruct>::Temp keys = Mem::fixed_container<MyStruct>(100);
    FixedBlock<MyStruct>::Temp vals = Mem::fixed_container<MyStruct>(100);
    Vector<U8*>::Temp reftracker = Data::vector<U8*>(50);

    for (int i = 0; i < 50; i++) {
        MyStruct* k =  Mem::alloc(&keys);
        k->a = i * 2;
        k->b = i * 4;

        MyStruct* v = Mem::alloc(&vals);
        v->a = i * 20;
        v->b = i * 40;

        Data::push_back(reftracker, (U8*) k);
        ReferenceTable::insert(tab, {(U8*) k, (U8*) v});
    }

    // for (int i = 0; i < 50; i++) {
    //     U8* key = *Data::at(reftracker, i);

    //     MyStruct* kdat = (MyStruct*) key;

    //     printf("%i %i", kdat->a, kdat->b);
    
    //     MyStruct* val = (MyStruct*) ReferenceTable::find(tab, key);

    //     if (val == nullptr) {
    //         printf(" = null\n");
    //     } else {
    //         printf(" = %i %i\n", val->a, val->b);
    //     }
    // }

    ReferenceTable::iterate(tab, [](U8* k, U8* v) {
        MyStruct* key = (MyStruct*) k;
        MyStruct* val = (MyStruct*) v;

        if (key == nullptr) {
            printf("null");
        } else {
            printf("%i %i", key->a, key->b);
        }

        if (val == nullptr) {
            printf(" = null\n");
        } else {
            printf(" = %i %i\n", val->a, val->b);
        }
    });

    return 0;
}