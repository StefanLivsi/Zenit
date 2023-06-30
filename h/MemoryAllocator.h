#ifndef OSPROJEKAT_MEMORYALLOCATOR_H
#define OSPROJEKAT_MEMORYALLOCATOR_H
#include "../lib/console.h"
#include "../lib/hw.h"


//extern FreeMem* fmem_head;

class MemoryAllocator {
public:
    ~MemoryAllocator() = default;
    static void* mem_alloc(size_t size);
    static void * mem_alloc_wrapper(size_t size);
    static int mem_free(void* addr);
    static void init(void* spaceBegin, void* spaceEnd);
    struct FreeMem{
        size_t size;
        FreeMem* next;
    };

private:

    static FreeMem* fmem_head;
    static FreeMem  mymem;
    static void* MEM_SPACE_BEGIN;
    static void* MEM_SPACE_END;

    MemoryAllocator() = delete;
    MemoryAllocator(MemoryAllocator const&);
    void operator=(MemoryAllocator const&);
    static int tryToJoin(FreeMem* fm);
};

#endif //OSPROJEKAT_MEMORYALLOCATOR_H
