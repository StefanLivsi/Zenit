//
// Created by os on 8/17/22.
//

#ifndef OS_PROJEKAT_TESTOVI_MYBUFFER_H
#define OS_PROJEKAT_TESTOVI_MYBUFFER_H
//
// Created by zika on 2/26/22.
//

#ifndef OS1_BUFFER_CPP_H
#define OS1_BUFFER_CPP_H

#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.h"
#include "KernelObject.hpp"

class myBuffer : public KernelObject {
private:
    int cap;
    int *buffer;
    int head, tail;

    sem_t spaceAvailable;
    sem_t itemAvailable;
    sem_t mutexHead;
    sem_t mutexTail;

public:
    static myBuffer* createBuffer(int _cap);
    void init(int _cap);
    myBuffer(int _cap);
    myBuffer(){}
    ~myBuffer();

    void put(char val);
    char get();

    int getCnt();

    void operator delete(void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_myBuffer, p);
    }
    void operator delete[](void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_myBuffer, p);
    }
};


#endif //OS1_BUFFER_CPP_H


#endif //OS_PROJEKAT_TESTOVI_MYBUFFER_H
