//
// Created by os on 8/11/22.
//

#ifndef OSPROJEKAT_SEMAPHORE_H
#define OSPROJEKAT_SEMAPHORE_H
#include "List.h"
#include "tcb.h"
#include "KernelObject.hpp"

class mySemaphore : public KernelObject {
public:
    mySemaphore(int initValue);
    mySemaphore(){}
    static int initSem(mySemaphore* sem, int initValue);
    friend class Riscv;
    ~mySemaphore ();
    static mySemaphore* createSemaphore(int initValue = 1);
    void wait ();
    void signal ();
    int value () const { return val; };
    List<TCB> *blockedThreadQueue;

    void operator delete(void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_mySemaphore, p);
    }
    void operator delete[](void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_mySemaphore, p);
    }

protected:
    void block ();
    void unblock ();
    int val;
private:


};
#endif //OSPROJEKAT_SEMAPHORE_H
