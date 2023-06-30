//
// Created by os on 8/11/22.
//
#include "../h/Semaphore.h"
#include "../h/tcb.h"

mySemaphore::mySemaphore(int initValue) : val(initValue){
    blockedThreadQueue = (List<TCB>*)kmem_cache_alloc(SlabAllocator::cache_List);
}

void mySemaphore::wait() {
    if(--val < 0) block();

}

void mySemaphore::signal() {
    if(++val <= 0) unblock();
}

void mySemaphore::block() {
    blockedThreadQueue->addLast(TCB::running);
    TCB* old = TCB::running;
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void mySemaphore::unblock(){
    TCB *t = blockedThreadQueue->removeFirst();
    if(t)
    Scheduler::put(t);
}

mySemaphore::~mySemaphore() {
    delete[] blockedThreadQueue;
}
int mySemaphore::initSem(mySemaphore* sem, int initValue) {
    sem->val = initValue;
    //sem->blockedThreadQueue = new List<TCB>();
    sem->blockedThreadQueue = (List<TCB>*)kmem_cache_alloc(SlabAllocator::cache_List);
    if(!sem->blockedThreadQueue) return -1;
    return 0;
}

mySemaphore* mySemaphore::createSemaphore(int initValue) {
    //return new mySemaphore(initValue);
    mySemaphore* sem = (mySemaphore*)kmem_cache_alloc(SlabAllocator::cache_mySemaphore);
    if(initSem(sem,initValue)==-1) return nullptr;
    return sem;
}


