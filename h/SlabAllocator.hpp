//
// Created by os on 1/2/23.
//

#ifndef SUPEROS_SLABALLOCATOR_HPP
#define SUPEROS_SLABALLOCATOR_HPP
#include "slab.h"
typedef struct kmem_cache_s kmem_cache_t;
struct slabDesc;
class SlabAllocator{
private:
    static void* KERNELHEAP_START_ADDR;
    static int pageNum;
    static kmem_cache_t* cache_head;
    static void cacheInit(kmem_cache_t*, const char* name, size_t,void(*)(void *), void(*)(void*));
    static int slabCreate(kmem_cache_t* cache);
    static void slabDestroy(kmem_cache_t* cache, slabDesc* slabDesc);
    static kmem_cache_t* cache_bufferN[13];
    static void* last_returned_value;

public:
    SlabAllocator() = delete;
    static void init(void *space, int block_num);
    static kmem_cache_t* cacheCreate(const char* name, size_t size,
                             void (*ctor)(void *),
                             void (*dtor)(void *));
    static int cacheShrink(kmem_cache_t *cachep);
    static void* cacheAlloc(kmem_cache_t *cachep);
    static void cacheFree(kmem_cache_t *cachep, void * objp);
    static void *kmalloc(size_t size);
    static void kfree(const void *objp);
    static void cacheDestroy(kmem_cache_t *cachep);
    static void cacheInfo(kmem_cache_t *cachep);
    static int cacheError(kmem_cache_t *cachep);
    static void printCaches();
    static kmem_cache_t * cache_TCB;
    static kmem_cache_t * cache_mySemaphore;
    static kmem_cache_t * cache_List;
    static kmem_cache_t * cache_Elem;
    static kmem_cache_t * cache_myBuffer;
    static const char* names_cache[13];
};
#endif //SUPEROS_SLABALLOCATOR_HPP
