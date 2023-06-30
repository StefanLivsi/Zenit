//
// Created by marko on 20.4.22..
//

#include "../h/tcb.h"


TCB* TCB::running = nullptr;

List<TCB> TCB::periodicThreads;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, void* arg, int sMode)
{
    //return new TCB(body, TIME_SLICE, arg, sMode);
    TCB* t = (TCB*)kmem_cache_alloc(SlabAllocator::cache_TCB);

    if(initThread(t,body,arg,sMode)==-1) return nullptr;
    return t;
}

TCB* TCB::createThreadEmpty(){
    return (TCB*) kmem_cache_alloc(SlabAllocator::cache_TCB);
}

void TCB::yield()
{
    __asm__ volatile("addi a0, x0, 3");
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::superthreadWrapper()
{
    running->body();
    running->setFinished(true);
    TCB::yield();
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    __asm__ volatile("mv a0, %0" : : "r" (running->arg));
    running->body();
    running->setFinished(true);
    TCB::yield();
}

void TCB::threadWrapper2()
{
    Riscv::popSppSpie();
    //__asm__ volatile("mv a0, %0" : : "r" (running->arg));

    do{
        running->myThread->run();
        time_sleep(TCB::running->periodicTime);
    }
    while(TCB::running->periodicTime);
    running->setFinished(true);
    TCB::yield();
}

void TCB::swapSpSsp1(){
    if(!(Riscv::r_sstatus() & Riscv::BitMaskSstatus::SSTATUS_SPP)){
        uint64 spTemp;
        __asm__ volatile("mv %0, sp" : "=r"(spTemp));
        spTemp+=32;
        TCB::running->context.sp = TCB::running->context.ssp;
        TCB::running->context.ssp = spTemp;
        __asm__ volatile ("ld s0, 24(sp)");
        __asm__ volatile ("mv sp, %0" : : "r"(TCB::running->context.sp));
        __asm__ volatile ("ret");
    }
}

void TCB::swapSpSsp2() {
    if(!(Riscv::r_sstatus() & Riscv::BitMaskSstatus::SSTATUS_SPP)){
        uint64 spTemp;
        __asm__ volatile("mv %0, sp" : "=r"(spTemp));
        spTemp+=32;
        TCB::running->context.sp = TCB::running->context.ssp;
        TCB::running->context.ssp = spTemp;
        __asm__ volatile ("ld s0, 24(sp)");
        __asm__ volatile ("mv sp, %0" : : "r"(TCB::running->context.sp));
        __asm__ volatile ("ret");
    }
}

int TCB::exitThread(){
    delete TCB::running->myThread;
    delete TCB::running;
    running = Scheduler::get();
    if(!running) return -1;//ovo je sranje
    __asm__ volatile("mv sp, %0" : : "r"(TCB::running->context.sp));
    __asm__ volatile("mv ra, %0" : : "r"(TCB::running->context.ra));
    __asm__ volatile("ret");
    return 1;
}

int TCB::sleepThread(time_t time) {
    if(time==0) return -1;
    TCB *old = TCB::running;
    TCB::running->sleepTime = time;
    Scheduler::putSleep(TCB::running);
    running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);

    return 0;
}

int TCB::initThread(thread_t t, TCB::Body body, void *arg1, int sMode) {
    t->body = body;
    //t->stack = ((body != nullptr) || CPPAPI) ? (uint64 *) MemoryAllocator::mem_alloc_wrapper(SIZE_ALLOC) : nullptr;
    //t->sstack = ((body != nullptr) || CPPAPI) ? (uint64 *) kmalloc(SIZE_ALLOC) : nullptr;
    t->stack = ((body != nullptr) || CPPAPI) ? (uint64 *) MemoryAllocator::mem_alloc_wrapper(SIZE_ALLOC) : nullptr;
    t->sstack = ((body != nullptr) || CPPAPI) ? (uint64 *) kmalloc(SIZE_ALLOC) : nullptr;
    t->mode = sMode;
    t->context = {t->mode == 0 ? (uint64) &threadWrapper : (uint64) &superthreadWrapper,
                t->stack != nullptr ? (uint64) &t->stack[STACK_SIZE] : 0,
                t->sstack != nullptr ? (uint64) &t->sstack[STACK_SIZE] : 0
               };
    t->timeSlice = TCB::TIME_SLICE,
    t->finished = false,
    t->sleepTime = 0,
    t->arg = arg1;
    if((body != nullptr) || CPPAPI)
        if(!t->stack || !t->sstack) return -1;
    if(!CPPAPI && (body!=nullptr)) {

        Scheduler::put(t);
    }
    return 0;
}
