//
// Created by os on 12/22/22.
//

#ifndef SUPEROS_SLAB_H
#define SUPEROS_SLAB_H
#include "../lib/hw.h"
//#include <new>
#include "../h/SlabAllocator.hpp"
class SlabAllocator;

struct kmem_cache_s;
struct slabDesc;
struct slot{
    slot* next;
    slabDesc* head;
};
struct slabDesc{
    kmem_cache_s* owner;//slabDesc pripada ovom kesu
    slot* head;
    slabDesc* next;
    unsigned int inuse; // broj iskoriscenih slotova
};
struct kmem_cache_s{
    void *operator new(size_t, void* addr){
        return addr;
    }
    const char* name; // ime kesa
    slabDesc* slabsEmpty;
    slabDesc* slabsFull;
    slabDesc* slabsPartial;
    size_t objSize; // velicina objekta koji se alocira
    void (*ctor)(void *); // POKAZIVAC NA LAMBDA FUNKCIJU KOJA RADI PLACEMENT NEW
    void (*dtor)(void *); //
    kmem_cache_s* next;
};

typedef struct kmem_cache_s kmem_cache_t;

static kmem_cache_t cache_cache{
        "cache_cache",
        nullptr,
        nullptr,
        nullptr,
        sizeof (kmem_cache_s),
        [](void* obj){ new (obj) kmem_cache_s;},
   //     [](void* obj){ ((kmem_cache_s*)obj)->~kmem_cache_s();},
        nullptr,
        nullptr
};
//static kmem_cache_t* cache_mySemaphore;

#define BLOCK_SIZE (4096)
void kmem_init(void *space, int block_num);
kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                void (*ctor)(void *),
                                void (*dtor)(void *)); // Allocate cache
int kmem_cache_shrink(kmem_cache_t *cachep); // Shrink cache
void *kmem_cache_alloc(kmem_cache_t *cachep); // Allocate one object from cache
void kmem_cache_free(kmem_cache_t *cachep, void *objp); // Deallocate one object from cache
void *kmalloc(size_t size); // Alloacate one small memory buffer
void kfree(const void *objp); // Deallocate one small memory buffer
void kmem_cache_destroy(kmem_cache_t *cachep); // Deallocate cache
void kmem_cache_info(kmem_cache_t *cachep); // Print cache info
int kmem_cache_error(kmem_cache_t *cachep); // Print error message

#endif //SUPEROS_SLAB_H
