//
// Created by os on 12/20/22.
//

#ifndef SUPEROS_KERNELOBJECT_HPP
#define SUPEROS_KERNELOBJECT_HPP
#include "../h/MemoryAllocator.h"
class KernelObject{
public:
    void *operator new(size_t, void* addr){
        return addr;
    }
    void *operator new(size_t n)
    {
        //size_t newsize = (n+sizeof(FreeMem))/MEM_BLOCK_SIZE;
        //if((n+sizeof(FreeMem)) % MEM_BLOCK_SIZE != 0) newsize++;
        return MemoryAllocator::mem_alloc_wrapper(n);
    }

    void *operator new[](size_t n)
    {
        //size_t newsize = (n+sizeof(FreeMem))/MEM_BLOCK_SIZE;
        //if((n+sizeof(FreeMem)) % MEM_BLOCK_SIZE != 0) newsize++;
        return MemoryAllocator::mem_alloc_wrapper(n);
    }

    void operator delete(void *p) noexcept
    {
        MemoryAllocator::mem_free(p);

    }

    void operator delete[](void *p) noexcept
    {
        MemoryAllocator::mem_free(p);
    }
};
#endif //SUPEROS_KERNELOBJECT_HPP
