// #define STD_HEAP_TRACK
#include "nstd.hpp"

using namespace nstd;



struct ReferenceTable {
    struct Item {
        Pair region;
        Item* next;
    };

    Hash::Group hash_type;
    DynamicBlock<Item> allocator; 
    List<Item*> references;

    static ReferenceTable* create(USZ width, Hash::Group hash_type = Hash::djb2) {
        ReferenceTable* table = Mem::type_alloc<ReferenceTable>();
        table->allocator = Mem::dynamic_container<Item>(width);
        table->references = Data::list<Item*>(width);
        table->hash_type = hash_type;

        return table;
    }

    static void destroy(ReferenceTable* table) {
        Mem::dealloc(&table->allocator);
        Mem::dealloc(&table->references);
        Mem::dealloc(table);
    }

    static Item* get_item(ReferenceTable* table, const U8* ptr) {
        USZ ptr_val = (USZ) ptr;
        USZ hash = Hash::generate((U8*) &ptr_val, sizeof(USZ), table->hash_type) % Data::size(table->references);
        return *Data::at(table->references, hash);
    }

    static void insert(ReferenceTable* table, Pair data) {

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

    static U8* find(ReferenceTable* table, const U8* lptr) {
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

    static void iterate(ReferenceTable* table, void (*it)(U8*, U8*)) {
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
};

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