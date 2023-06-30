#include "../h/slab.h"

void kmem_init(void *space, int block_num){
    SlabAllocator::init(space,block_num);
}

kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                void (*ctor)(void *),
                                void (*dtor)(void *)){
    return SlabAllocator::cacheCreate((name), size, ctor, dtor);
}
void *kmem_cache_alloc(kmem_cache_t *cachep){
    return SlabAllocator::cacheAlloc(cachep);
}

void kmem_cache_free(kmem_cache_t *cachep, void *objp){
    SlabAllocator::cacheFree(cachep,objp);
}
int kmem_cache_shrink(kmem_cache_t *cachep){
    return SlabAllocator::cacheShrink(cachep);
}
void *kmalloc(size_t size){
    return SlabAllocator::kmalloc(size);
}
void kfree(const void *objp){
    SlabAllocator::kfree(objp);
}
void kmem_cache_destroy(kmem_cache_t *cachep){
    SlabAllocator::cacheDestroy(cachep);
}
void kmem_cache_info(kmem_cache_t *cachep) {
    SlabAllocator::cacheInfo(cachep);
}
int kmem_cache_error(kmem_cache_t *cachep) {
    return SlabAllocator::cacheError(cachep);
}