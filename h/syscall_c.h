//
// Created by os on 5/16/22.
//

#ifndef OSPROJEKAT_SYSCALL_C_H
#define OSPROJEKAT_SYSCALL_C_H
#include "../lib/hw.h"
#include "../h/List.h"
class TCB;
class mySemaphore;
typedef TCB* thread_t;
typedef mySemaphore* sem_t;
typedef unsigned long time_t;
const int EOF = -1;
struct args{
    sem_t* arg1;
    unsigned arg2;
    void* arg3;
};
struct args2{
    thread_t* t;
    void(*start_routine)();
    void* arg3;
};
extern "C" void* mem_alloc(size_t size);
extern "C" int mem_free(void*);
extern "C" void __printString(char const *string);
extern "C" void __printInteger(uint64 integer);
extern "C" void thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
extern "C" int thread_exit();
extern "C" int time_sleep(time_t);
extern "C" void thread_dispatch();
extern "C" void putc(char);
extern "C" char getc();
extern "C" int sem_open(sem_t* handle, unsigned init);
extern "C" int sem_close(sem_t handle);
extern "C" int sem_wait(sem_t id);
extern "C" int sem_signal(sem_t id);

#endif //OSPROJEKAT_SYSCALL_C_H
