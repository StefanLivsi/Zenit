//
// Created by os on 1/4/23.
//

#ifndef SUPEROS_BUDDYALLOCATOR_HPP
#define SUPEROS_BUDDYALLOCATOR_HPP
#include "../lib/hw.h"
/*
#define BUCKET_SIZE 13
class BuddyAllocator{
private:
    static uint8* BUDDY_HEAP_BEGIN;
    static size_t block_num;
    typedef struct Elem {
        struct Elem *prev, *next;
        static void list_init(Elem* list){
            list->prev = list;
            list->next = list;
        }
        static void put(Elem* list, Elem* entry){
            Elem *prev = list->prev;
            entry->prev = prev;
            entry->next = list;
            prev->next = entry;
            list->prev = entry;
        }
        static void remove(Elem* entry){
            Elem *prev = entry->prev;
            Elem *next = entry->next;
            prev->next = next;
            next->prev = prev;
        }
        static Elem* get(Elem* list){
            Elem *back = list->prev;
            if (back == list) return nullptr;
            remove(back);
            return back;
        }
    } elem;
    Elem* head;
    Elem* tail;
    static Elem bucket[BUCKET_SIZE];
    *//*class List {

    public:
        List () : head(nullptr), tail(nullptr){}
        void put(Elem* entry){
            Elem* prev = entry->prev;
            Elem* next = entry->next;
            prev->next = next;
            next->prev = prev;
        }
        Elem* get(){
            Elem* back =
        }

        //void put (char* p) { *(char**)p = head; head = p; }
        //char* get () { char* p = head; if (head) head = *(char**)head; return p; }
        *//**//*void put(char* p){
            if(!tail){
                head = tail = (Elem*)p;
            }
            else{
                tail->next = (Elem*)p;
                tail = (Elem*)p;
            }
        }
        char* get(){
            if(!tail) return nullptr;
            Elem* temp = tail;
            if(head == tail) head = tail = nullptr;
            else tail = tail->prev;
            return (char*)temp;
        }
        void remove(char* p){
            Elem* temp = head;
            while(temp){
                if(temp == (Elem*)p){
                    if(head==tail){ head = tail = nullptr; return;}
                    if(temp == head) {
                        head = temp->next;
                        return;
                    }
                    if(temp == tail){
                        tail = temp->prev;
                        return;
                    }
                    temp->prev->next = temp->next;
                    return;
                }
                temp = temp->next;
            }

        }*//**//*
   *//**//* private:
        typedef struct Elem {
            struct Elem *prev, *next;
        } elem;
        Elem* head;
        Elem* tail;*//**//*
    };*//*
    //static List bucket[BUCKET_SIZE];
    static uint8 node_is_split[(1 << (BUCKET_SIZE - 1))/8]; // 2^12 == BLOCK_SIZE, svaki blok dobije svoj bit

public:
    static void *alloc(size_t i);
    static void free(void* obj, size_t blocks);

    static void init(void *pVoid, int num);

    static void split(uint8 *seg, int upper, int lower, int i);

    static uint8 *ptr_for_node(size_t index, size_t bucket);

    static size_t node_for_ptr(uint8 *ptr, size_t bucket);

    static int parent_is_split(size_t index);

    static void flip_parent_is_split(size_t index);

    static size_t bucket_for_request(size_t request);
};*/
#define HEADER_SIZE 8

#define MIN_ALLOC_LOG2 12
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)

#define MAX_ALLOC_LOG2 24
#define MAX_ALLOC ((size_t)1 << MAX_ALLOC_LOG2)

#define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

typedef struct list_t {
    struct list_t *prev, *next;
} list_t;
class BuddyAllocator{
    static list_t buckets[BUCKET_COUNT];
    static size_t bucket_limit;
    static uint8 node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];
    static uint8 *base_ptr;
    static int update_max_ptr(uint8 *new_value);
    static void list_init(list_t *list);
    static void list_push(list_t *list, list_t *entry);
    static void list_remove(list_t *entry);
    static list_t *list_pop(list_t *list);
    static uint8 *ptr_for_node(size_t index, size_t bucket);
    static size_t node_for_ptr(uint8 *ptr, size_t bucket);
    static int parent_is_split(size_t index);
    static void flip_parent_is_split(size_t index);
    static size_t bucket_for_request(size_t request);
    static int lower_bucket_limit(size_t bucket);

public:
    static void *malloc(size_t request);

    static void free(void *ptr);
};

/*


static list_t buckets[BUCKET_COUNT];

static size_t bucket_limit;

static uint8 node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];


static uint8 *base_ptr;

static uint8 *max_ptr;


static int update_max_ptr(uint8 *new_value) {
    *//*if (new_value > max_ptr) {
        if (brk(new_value)) {
            return 0;
        }
        max_ptr = new_value;
    }
    return 1;*//*
    return 1;
}


static void list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}


static void list_push(list_t *list, list_t *entry) {
    list_t *prev = list->prev;
    entry->prev = prev;
    entry->next = list;
    prev->next = entry;
    list->prev = entry;
}


static void list_remove(list_t *entry) {
    list_t *prev = entry->prev;
    list_t *next = entry->next;
    prev->next = next;
    next->prev = prev;
}

static list_t *list_pop(list_t *list) {
    list_t *back = list->prev;
    if (back == list) return nullptr;
    list_remove(back);
    return back;
}

static uint8 *ptr_for_node(size_t index, size_t bucket) {
    return base_ptr + ((index - (1 << bucket) + 1) << (MAX_ALLOC_LOG2 - bucket));
}

static size_t node_for_ptr(uint8 *ptr, size_t bucket) {
    return ((ptr - base_ptr) >> (MAX_ALLOC_LOG2 - bucket)) + (1 << bucket) - 1;
}

static int parent_is_split(size_t index) {
    index = (index - 1) / 2;
    return (node_is_split[index / 8] >> (index % 8)) & 1;
}


static void flip_parent_is_split(size_t index) {
    index = (index - 1) / 2;
    node_is_split[index / 8] ^= 1 << (index % 8);
}


static size_t bucket_for_request(size_t request) {
    size_t bucket = BUCKET_COUNT - 1;
    size_t size = MIN_ALLOC;

    while (size < request) {
        bucket--;
        size *= 2;
    }

    return bucket;
}


static int lower_bucket_limit(size_t bucket) {
    while (bucket < bucket_limit) {
        size_t root = node_for_ptr(base_ptr, bucket_limit);
        uint8 *right_child;

        if (!parent_is_split(root)) {
            list_remove((list_t *)base_ptr);
            list_init(&buckets[--bucket_limit]);
            list_push(&buckets[bucket_limit], (list_t *)base_ptr);
            continue;
        }

        right_child = ptr_for_node(root + 1, bucket_limit);
        if (!update_max_ptr(right_child + sizeof(list_t))) {
            return 0;
        }
        list_push(&buckets[bucket_limit], (list_t *)right_child);
        list_init(&buckets[--bucket_limit]);

        root = (root - 1) / 2;
        if (root != 0) {
            flip_parent_is_split(root);
        }
    }

    return 1;
}*/

/*void *malloc(size_t request) {
    size_t original_bucket, bucket;

    if (request + HEADER_SIZE > MAX_ALLOC) {
        return nullptr;
    }

    if (base_ptr == nullptr) {
        //base_ptr = max_ptr = (uint8 *)sbrk(0);
        base_ptr = (uint8*)HEAP_START_ADDR;
        max_ptr = (uint8*)((size_t)HEAP_START_ADDR + (4096*4096));
        bucket_limit = BUCKET_COUNT - 1;
        //update_max_ptr(base_ptr + sizeof(list_t));
        list_init(&buckets[BUCKET_COUNT - 1]);
        list_push(&buckets[BUCKET_COUNT - 1], (list_t *)base_ptr);
    }


    bucket = bucket_for_request(request + HEADER_SIZE);
    original_bucket = bucket;


    while (bucket + 1 != 0) {
        size_t size, bytes_needed, i;
        uint8 *ptr;


        if (!lower_bucket_limit(bucket)) {
            return nullptr;
        }


        ptr = (uint8 *)list_pop(&buckets[bucket]);
        if (!ptr) {

            if (bucket != bucket_limit || bucket == 0) {
                bucket--;
                continue;
            }


            if (!lower_bucket_limit(bucket - 1)) {
                return nullptr;
            }
            ptr = (uint8 *)list_pop(&buckets[bucket]);
        }

        size = (size_t)1 << (MAX_ALLOC_LOG2 - bucket);
        bytes_needed = bucket < original_bucket ? size / 2 + sizeof(list_t) : size;
        if (!update_max_ptr(ptr + bytes_needed)) {
            list_push(&buckets[bucket], (list_t *)ptr);
            return nullptr;
        }

        i = node_for_ptr(ptr, bucket);
        if (i != 0) {
            flip_parent_is_split(i);
        }

        while (bucket < original_bucket) {
            i = i * 2 + 1;
            bucket++;
            flip_parent_is_split(i);
            list_push(&buckets[bucket], (list_t *)ptr_for_node(i + 1, bucket));
        }

        *(size_t *)ptr = request;
        return ptr + HEADER_SIZE;
    }

    return nullptr;
}

void free(void *ptr) {
    size_t bucket, i;

    if (!ptr) {
        return;
    }

    ptr = (uint8 *)ptr - HEADER_SIZE;
    bucket = bucket_for_request(*(size_t *)ptr + HEADER_SIZE);
    i = node_for_ptr((uint8 *)ptr, bucket);

    while (i != 0) {

        flip_parent_is_split(i);

        if (parent_is_split(i) || bucket == bucket_limit) {
            break;
        }

        list_remove((list_t *)ptr_for_node(((i - 1) ^ 1) + 1, bucket));
        i = (i - 1) / 2;
        bucket--;
    }

    list_push(&buckets[bucket], (list_t *)ptr_for_node(i, bucket));
}*/
#endif //SUPEROS_BUDDYALLOCATOR_HPP
