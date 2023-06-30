//
// Created by os on 1/3/23.
//
#include "../h/SlabAllocator.hpp"
#include "../h/BuddyAllocator.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/print.h"
#include "../h/tcb.h"
#include "../h/Semaphore.h"
#include "../h/myBuffer.h"
void* SlabAllocator::KERNELHEAP_START_ADDR;
int SlabAllocator::pageNum;
kmem_cache_t* SlabAllocator::cache_head = &cache_cache;
kmem_cache_t* SlabAllocator::cache_bufferN[13];
kmem_cache_t* SlabAllocator::cache_TCB;
kmem_cache_t* SlabAllocator::cache_mySemaphore;
kmem_cache_t* SlabAllocator::cache_List;
kmem_cache_t* SlabAllocator::cache_Elem;
kmem_cache_t* SlabAllocator::cache_myBuffer;
void* SlabAllocator::last_returned_value;
const char* SlabAllocator::names_cache[]{"size-5","size-6","size-7","size-8","size-9","size-10","size-11","size-12","size-13","size-14","size-15","size-16","size-17"};
void SlabAllocator::init(void *space, int block_num) {
    KERNELHEAP_START_ADDR = space;
    pageNum = block_num;
    size_t size = 32;
    for(int i = 0; i < 13; i++){
        cache_bufferN[i] = cacheCreate(names_cache[i], size, nullptr, nullptr);
        size <<= 1;
    }
    cache_TCB = cacheCreate("cache_TCB", sizeof(TCB),[](void* obj){new (obj) TCB;},[](void* obj){((TCB*)obj)->~TCB();});
    cache_mySemaphore = cacheCreate("cache_mySemaphore", sizeof(mySemaphore),[](void* obj){new (obj) mySemaphore;},[](void* obj){((mySemaphore*)obj)->~mySemaphore();});
    cache_List = cacheCreate("cache_List", sizeof(List<TCB>),[](void* obj){new (obj) List<TCB>;}, nullptr);
    cache_Elem = cacheCreate("cache_Elem",sizeof(List<TCB>::Elem), [](void* obj) {new (obj) List<TCB>;},nullptr);
    cache_myBuffer = cacheCreate("cache_myBuffer",sizeof(myBuffer), [](void* obj) {new (obj) myBuffer;},[](void* obj){((myBuffer*)obj)->~myBuffer();});
}

kmem_cache_t* SlabAllocator::cacheCreate(const char* name, size_t size, void (*ctor)(void *), void (*dtor)(void *)) {
    kmem_cache_t* newCache = (kmem_cache_t*) cacheAlloc(&cache_cache);
    cacheInit(newCache,name,size,ctor,dtor);
    newCache->next = cache_head->next;
    cache_head->next = newCache;
    return newCache;
}

int SlabAllocator::cacheShrink(kmem_cache_t *cachep) {
    slabDesc* mySlabDesc = cachep->slabsEmpty;
    int cnt = 0;
    while(mySlabDesc){
        slabDesc* temp = mySlabDesc;
        mySlabDesc = mySlabDesc->next;
        slabDestroy(cachep, temp);
        cnt++;
    }
    cachep->slabsEmpty = nullptr;
    return cnt;
}

void *SlabAllocator::cacheAlloc(kmem_cache_t *cachep) {
    if(slabDesc* curSlab = cachep->slabsPartial){
        slot* cur = curSlab->head;
        curSlab->head = curSlab->head->next;
        if(!curSlab->head) {
            cachep->slabsPartial = curSlab->next;
            if(cachep->slabsFull) {
                curSlab->next = cachep->slabsFull->next;
                cachep->slabsFull->next = curSlab;
            }
            else{
                cachep->slabsFull = curSlab;
                curSlab->next = nullptr;
            }
        }
        curSlab->inuse++;
        last_returned_value = (void*) ((char*)cur + sizeof(slot));
        return (void*) ((char*)cur + sizeof(slot));

    }

    else if(slabDesc* curSlab = cachep->slabsEmpty){
        slot* cur = curSlab->head;
        curSlab->head = curSlab->head->next;
        cachep->slabsEmpty = cachep->slabsEmpty->next;
        if(cachep->slabsPartial){
            curSlab->next = cachep->slabsPartial->next;
            cachep->slabsPartial->next = curSlab;
        }
        else{
            cachep->slabsPartial = curSlab;
            curSlab->next = nullptr;
        }
        curSlab->inuse++;
        last_returned_value = (void*) ((char*)cur + sizeof(slot));
        return (void*) ((char*)cur + sizeof(slot));
    }
    else{
        if(slabCreate(cachep)==-1) return nullptr;
        curSlab = cachep->slabsEmpty;
        cachep->slabsEmpty = cachep->slabsEmpty->next;

        slot* cur = curSlab->head;
        curSlab->head = curSlab->head->next;
        curSlab->inuse++;
        if(cur->next) {
            if (cachep->slabsPartial) {
                curSlab->next = cachep->slabsPartial->next;
                cachep->slabsPartial->next = curSlab;
            } else {
                cachep->slabsPartial = curSlab;
                curSlab->next = nullptr;
            }
        }
        else{
            if (cachep->slabsFull) {
                curSlab->next = cachep->slabsFull->next;
                cachep->slabsFull->next = curSlab;
            } else {
                cachep->slabsFull = curSlab;
                curSlab->next = nullptr;
            }
        }
        last_returned_value = (void*) ((char*)cur + sizeof(slot));
        return (void*) ((char*)cur + sizeof(slot));
    }
    return nullptr;
}

void SlabAllocator::cacheFree(kmem_cache_t *cachep, void *objp) {
    slot* mySlot = (slot*)((char*)objp - sizeof(slot));
    slabDesc* mySlabDesc = mySlot->head;
    if(mySlabDesc->head == nullptr){
        slabDesc* prevSlab = nullptr;
        slabDesc* cacheListSlab = cachep->slabsFull;
        while(mySlabDesc!=cacheListSlab){
            prevSlab=cacheListSlab;
            cacheListSlab = cacheListSlab->next;
        }
        if(!prevSlab)
            cachep->slabsFull = mySlabDesc->next;
        else{
            prevSlab->next = mySlabDesc->next;
        }
        if(cachep->slabsPartial) {
            mySlabDesc->next = cachep->slabsPartial->next;
            cachep->slabsPartial->next = mySlabDesc;
        }
        else{
            cachep->slabsPartial = mySlabDesc;
            mySlabDesc->next = nullptr;
        }
    }
    else if(mySlabDesc->inuse == 1){
        slabDesc* prevSlab = nullptr;
        slabDesc* cacheListSlab = cachep->slabsPartial;
        while(mySlabDesc!=cacheListSlab){
            prevSlab=cacheListSlab;
            cacheListSlab = cacheListSlab->next;
        }
        if(!prevSlab)

            cachep->slabsPartial = mySlabDesc->next;
        else{
            prevSlab->next = mySlabDesc->next;

        }
        if(cachep->slabsEmpty) {
            mySlabDesc->next = cachep->slabsEmpty->next;
            cachep->slabsEmpty->next = mySlabDesc;
        }
        else{
            cachep->slabsEmpty = mySlabDesc;
            mySlabDesc->next = nullptr;
        }
    }

    if(mySlabDesc->head){
        mySlot->next = mySlabDesc->head->next;
        mySlabDesc->head->next = mySlot;
    }
    else{
        mySlabDesc->head = mySlot;
        mySlot->next = nullptr;
    }


    mySlabDesc->inuse--;

}
void *SlabAllocator::kmalloc(size_t size) { // size u bajtovima
    size_t value  =  1;
    int power = 0;
    while  ( value  <  size)
    {
        power++;
        value  =  value  <<  1 ;
    }
    if(power < 5) power = 5;
    if(power > 17) return nullptr;
    return cacheAlloc(cache_bufferN[power-5]);
}

void SlabAllocator::kfree(const void *objp) {
    slot* mySlot = (slot*) ((char*)objp - sizeof (slot));
    kmem_cache_t* cache = mySlot->head->owner;
    cacheFree(cache, const_cast<void *>(objp));
}

void SlabAllocator::cacheDestroy(kmem_cache_t *cachep) {
    slabDesc* myDesc = cachep->slabsFull;
    while(myDesc){
        slabDesc* temp = myDesc;
        slabDestroy(cachep,temp);
        myDesc = myDesc->next;
    }
    myDesc = cachep->slabsPartial;
    while(myDesc){
        slabDesc* temp = myDesc;
        slabDestroy(cachep,temp);
        myDesc = myDesc->next;
    }
    myDesc = cachep->slabsEmpty;
    while(myDesc){
        slabDesc* temp = myDesc;
        slabDestroy(cachep,temp);
        myDesc = myDesc->next;
    }
    kmem_cache_t * prev = nullptr;
    kmem_cache_t * cur = &cache_cache;
    while(cur!=cachep){
        prev = cur;
        cur = cur->next;
    }
    prev->next = cur->next;
    kmem_cache_free(&cache_cache,(void*)cachep);
}

void SlabAllocator::cacheInfo(kmem_cache_t *cachep) {
    size_t inUse = 0;
    slabDesc* desc = cachep->slabsFull;
    int numOfSlabs = 0;
    int numOfPagesPerSlab = 0;
    int numOfObjInSlab = 0;
    int percentageWhole = 0;
    int percentageDecimal = 0;
    int numOfPages = 0;
    while(desc){
        inUse+=desc->inuse;
        numOfSlabs++;
        desc = desc->next;
    }
    desc = cachep->slabsPartial;
    while(desc){
        inUse+=desc->inuse;
        numOfSlabs++;
        desc = desc->next;
    }
    desc = cachep->slabsEmpty;
    while(desc){
        numOfSlabs++;
        desc = desc->next;
    }
    numOfPagesPerSlab = (cachep->objSize + sizeof(slabDesc) + sizeof (slot))/BLOCK_SIZE;
    if((cachep->objSize + sizeof(slabDesc) + sizeof (slot))%BLOCK_SIZE != 0) numOfPagesPerSlab++;
    numOfPages = numOfSlabs * numOfPagesPerSlab;
    if(numOfPagesPerSlab == 1) numOfObjInSlab = (BLOCK_SIZE - sizeof(slabDesc))/(cachep->objSize + sizeof(slot));
    else numOfObjInSlab = 1;
    if(numOfSlabs==0)
        percentageWhole = 0;
    else{
        percentageWhole = (inUse * 100)/ (numOfObjInSlab*numOfSlabs);
        percentageDecimal = (inUse * 100) % (numOfObjInSlab*numOfSlabs);
    }

    char abc[30];
    const char* temp = cachep->name;
    for(int i = 0; i < 30; i++){
        if(*temp == '\0'){
            abc[i] = ' ';
        }
        else{
            abc[i] = *temp;
            temp++;
        }
    }
    oldprintString(abc);
    oldprintInteger(cachep->objSize);
    oldprintString("\t");
    oldprintInteger(numOfPages);
    oldprintString("\t");
    oldprintInteger(numOfSlabs);
    oldprintString("\t");
    oldprintInteger(numOfObjInSlab);
    oldprintString("\t");
    oldprintInteger(percentageWhole);
    oldprintString(".");
    oldprintInteger(percentageDecimal);
    oldprintString("\n");
}

void SlabAllocator::printCaches() {
    kmem_cache_t* cur = cache_head;
    while(cur){
        cacheInfo(cur);
        cur = cur->next;
    }
}

int SlabAllocator::cacheError(kmem_cache_t *cachep) {
    return !last_returned_value ? -1 : 0;
}

void
SlabAllocator::cacheInit(kmem_cache_t* cache, const char *name,size_t size, void (*ctor)(void *),
                         void (*dtor)(void *)) {
    cache->name = name;
    cache->objSize = size;
    cache->ctor = ctor;
    cache->dtor = dtor;
    cache->slabsPartial = nullptr;
    cache->slabsEmpty = nullptr;
    cache->slabsFull = nullptr;
}

int SlabAllocator::slabCreate(kmem_cache_t *cachep) {
    //size_t sizeToAlloc = (sizeof(slabDesc) + cachep->objSize + sizeof(slot)) > (BLOCK_SIZE-16) ? (sizeof(slabDesc) + cachep->objSize + sizeof(slot))*8: BLOCK_SIZE-16;
    size_t minSize = sizeof(slabDesc) + cachep->objSize + sizeof (slot);
    size_t sizeToAlloc = minSize > BLOCK_SIZE ? minSize : BLOCK_SIZE;
    void* slab = BuddyAllocator::malloc(sizeToAlloc);
    if(!slab)
        return -1;
    slabDesc* slabdesc = (slabDesc*) slab;
    slabdesc->next=nullptr;
    slabdesc->head = (slot*)((char*)slab + sizeof(slabDesc));
    slabdesc->owner = cachep;
    slabdesc->inuse = 0;
    if(cachep->slabsEmpty){
        slabdesc->next=cachep->slabsEmpty->next;
        cachep->slabsEmpty->next = slabdesc;
    }
    else{
        cachep->slabsEmpty = slabdesc;
        slabdesc->next = nullptr;
    }
    size_t mySize = sizeToAlloc;
    mySize -= sizeof(slabDesc);
    slot* curSlot = slabdesc->head;
    size_t sizeDec = sizeof(slot)+cachep->objSize;
    while(mySize >= sizeDec){
        mySize -= sizeDec;
        if(cachep->ctor) {
            void *ctorAddress = (char *) curSlot + sizeof(slot);
            cachep->ctor(ctorAddress);
        }
        curSlot->head = slabdesc;
        if(mySize>=sizeDec){
            curSlot->next = (slot*)((char*)curSlot+sizeDec);
            curSlot = curSlot->next;
        }
        else
            curSlot->next = nullptr;
    }
    return 1;
}

void SlabAllocator::slabDestroy(kmem_cache_t *cachep, slabDesc *mySlabDesc) {

    if(cachep->dtor){
        slot* mySlot = mySlabDesc->head;
        while(mySlot){
            void* objAddr = (char*)mySlot + sizeof(slot);
            cachep->dtor(objAddr);
            mySlot = mySlot->next;
        }
    }
    BuddyAllocator::free((void*)mySlabDesc);
}










