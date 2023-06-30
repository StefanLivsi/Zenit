/*
#include "../h/BuddyAllocator.hpp"
#include "../h/SlabAllocator.hpp"
#include "../test/printing.hpp"
#include "../h/print.h"
uint8* BuddyAllocator::BUDDY_HEAP_BEGIN;
size_t BuddyAllocator::block_num;
BuddyAllocator::Elem BuddyAllocator::bucket[BUCKET_SIZE];
uint8 BuddyAllocator::node_is_split[(1 << (BUCKET_SIZE - 1))/8];

size_t BuddyAllocator::bucket_for_request(size_t request) {
    size_t bucket = 0;
    size_t size = BLOCK_SIZE;

    while (size < request) {
        bucket++;
        size *= 2;
    }

    return bucket;
}

uint8 *BuddyAllocator::ptr_for_node(size_t index, size_t bucket) {
    //return BUDDY_HEAP_BEGIN + ((index - (1 << bucket) + 1) << (BUCKET_SIZE - bucket));
    return BUDDY_HEAP_BEGIN + ((index - (1 << (BUCKET_SIZE - bucket)) + 1) << (bucket));
}

size_t BuddyAllocator::node_for_ptr(uint8 *ptr, size_t bucket) {
    return ((ptr - BUDDY_HEAP_BEGIN) >> (bucket)) + ((1 << BUCKET_SIZE) - bucket) - 1;
}


int BuddyAllocator::parent_is_split(size_t index) {
    index = (index - 1) / 2;
    return (node_is_split[index / 8] >> (index % 8)) & 1;
}

void BuddyAllocator::flip_parent_is_split(size_t index) {
    index = (index - 1) / 2;
    node_is_split[index / 8] ^= 1 << (index % 8);
}

void BuddyAllocator::init(void *pVoid, int num) {
    BUDDY_HEAP_BEGIN = (uint8*)pVoid;
    block_num = num;
    int value  =  1;
    int power = 0;
    while  ( value  <  num)
    {
        power++;
        value  =  value  <<  1 ;
    }
    size_t temp = BUCKET_SIZE;
    while(temp--){
        Elem::list_init(&bucket[temp]);
    }
    */
/*for(Elem elem : bucket) {
        Elem::list_init(&elem);
    }*//*

    //power--;
    //bucket[BUCKET_SIZE-1].put((char*)BUDDY_HEAP_BEGIN);
    Elem::put(&bucket[BUCKET_SIZE-1],(Elem*)BUDDY_HEAP_BEGIN);
}
void BuddyAllocator::split (uint8* seg, int upper, int lower, int i) {
    while (--upper>=lower){
        i = i / 2;
        flip_parent_is_split(i);
        //bucket[upper].put(seg + (1<<upper)*BLOCK_SIZE);
        Elem::put(&bucket[upper],(Elem*)(seg + (1<<upper)*BLOCK_SIZE));
    }


}
extern bool myCheck;
void *BuddyAllocator::alloc(size_t sizeBytes) {
    size_t size = bucket_for_request(sizeBytes);
    if (size>=BUCKET_SIZE) return nullptr;
    for (size_t current=size; current<BUCKET_SIZE; current++) {
        //char* p = bucket[current].get();
        //if(myCheck)
        //printString("OVDE");

        uint8* p = (uint8*)Elem::get(&bucket[current]);
       */
/* if(myCheck){
            for(int i = 0; i < 13; i++){
                printInt(i);
                printString("\t");
                Elem* temp = &bucket[i];
                Elem* movingTemp = &bucket[i];
                int cnt = 0;
                while(movingTemp->next != temp){
                    movingTemp = movingTemp->next;
                    cnt++;
                }
                printInt(cnt);
                printString("\n");

            }
            printInt((uint64)p);
            printString("\n");
        }*//*

        if (!p) continue;
        int i = node_for_ptr((uint8*)p,current);
        flip_parent_is_split(i);
        split(p,current,size,i);
        return p;
    }
    //printString("NULLPTR");
    return nullptr;
}

void BuddyAllocator::free(void *obj, size_t sizeBytes) {
    size_t i, myBucket = bucket_for_request(sizeBytes);
    i = node_for_ptr((uint8*)obj, myBucket);
    while(true){
        flip_parent_is_split(i);
        if(parent_is_split(i) || myBucket == BUCKET_SIZE)
            break;

        //bucket[myBucket].remove((char*) ptr_for_node((((i-1)^ 1) + 1),myBucket));
        Elem::remove((Elem*) ptr_for_node((((i-1)^ 1) + 1),myBucket));
        i = (i - 1) / 2;
        myBucket++;
    }
    //bucket[myBucket].put((char*)obj);
    Elem::put(&bucket[myBucket], (Elem*)obj);
}

*/
#include "../h/BuddyAllocator.hpp"
list_t BuddyAllocator::buckets[BUCKET_COUNT];
size_t BuddyAllocator::bucket_limit;
uint8 BuddyAllocator::node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];
uint8 *BuddyAllocator::base_ptr;


void BuddyAllocator::list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}


void BuddyAllocator::list_push(list_t *list, list_t *entry) {
    list_t *prev = list->prev;
    entry->prev = prev;
    entry->next = list;
    prev->next = entry;
    list->prev = entry;
}


void BuddyAllocator::list_remove(list_t *entry) {
    list_t *prev = entry->prev;
    list_t *next = entry->next;
    prev->next = next;
    next->prev = prev;
}

list_t *BuddyAllocator::list_pop(list_t *list) {
    list_t *back = list->prev;
    if (back == list) return nullptr;
    list_remove(back);
    return back;
}

uint8 *BuddyAllocator::ptr_for_node(size_t index, size_t bucket) {
    return base_ptr + ((index - (1 << bucket) + 1) << (MAX_ALLOC_LOG2 - bucket));
}

size_t BuddyAllocator:: node_for_ptr(uint8 *ptr, size_t bucket) {
    return ((ptr - base_ptr) >> (MAX_ALLOC_LOG2 - bucket)) + (1 << bucket) - 1;
}

int BuddyAllocator::parent_is_split(size_t index) {
    index = (index - 1) / 2;
    return (node_is_split[index / 8] >> (index % 8)) & 1;
}


void BuddyAllocator::flip_parent_is_split(size_t index) {
    index = (index - 1) / 2;
    node_is_split[index / 8] ^= 1 << (index % 8);
}

size_t BuddyAllocator::bucket_for_request(size_t request) {
    size_t bucket = BUCKET_COUNT - 1;
    size_t size = MIN_ALLOC;

    while (size < request) {
        bucket--;
        size *= 2;
    }

    return bucket;
}


int BuddyAllocator::lower_bucket_limit(size_t bucket) {
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
        list_push(&buckets[bucket_limit], (list_t *)right_child);
        list_init(&buckets[--bucket_limit]);

        root = (root - 1) / 2;
        if (root != 0) {
            flip_parent_is_split(root);
        }
    }

    return 1;
}

void *BuddyAllocator::malloc(size_t request) {
    size_t original_bucket, bucket;

    if (request + HEADER_SIZE > MAX_ALLOC) {
        return nullptr;
    }

    if (base_ptr == nullptr) {
        base_ptr = (uint8*)HEAP_START_ADDR;
        bucket_limit = BUCKET_COUNT - 1;

        list_init(&buckets[BUCKET_COUNT - 1]);
        list_push(&buckets[BUCKET_COUNT - 1], (list_t *)base_ptr);
    }


    bucket = bucket_for_request(request + HEADER_SIZE);
    original_bucket = bucket;


    while (bucket + 1 != 0) {
        size_t i;
        uint8 *ptr;

        lower_bucket_limit(bucket);

        ptr = (uint8 *)list_pop(&buckets[bucket]);
        if (!ptr) {

            if (bucket != bucket_limit || bucket == 0) {
                bucket--;
                continue;
            }
            lower_bucket_limit(bucket-1);
            ptr = (uint8 *)list_pop(&buckets[bucket]);
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

void BuddyAllocator::free(void *ptr) {
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
}
