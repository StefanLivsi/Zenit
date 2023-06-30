//
// Created by os on 8/19/22.
//
#include "../h/syscall_cpp.hpp"
#include "../h/Scheduler.h"
#include "../h/tcb.h"
#include "../h/Semaphore.h"
bool CPPAPI=false;


Thread::Thread(void (*body)(void *), void *arg) {
    CPPAPI=true;
    thread_create(&myHandle,body,arg);
}

int Thread::start() {
    if(!myHandle) return -1;
    Scheduler::put(myHandle);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::Thread() {
    CPPAPI = true;
    thread_create(&myHandle,nullptr, nullptr);
    if(myHandle)
    myHandle->setWrapper(this);
}

Thread::~Thread() {
    if(myHandle){
        kmem_cache_free(SlabAllocator::cache_TCB,myHandle);
        //delete myHandle;
    }
}

Semaphore::Semaphore(unsigned int init) {
    //myHandle = new mySemaphore(init);
    //myHandle = mySemaphore::createSemaphore(init);
    sem_open(&myHandle,init);
}

int Semaphore::wait() {
    int a = sem_wait(myHandle);
    return a;
    //return sem_wait(myHandle);
}

int Semaphore::signal() {
    int a = sem_signal(myHandle);
    return a;
    //return sem_signal(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

PeriodicThread::PeriodicThread(time_t period) : Thread(){
    myHandle->setPeriodicwrapper(period);
}

void PeriodicThread::stopThread() {
    myHandle->setPeriodicwrapper(0);
}
