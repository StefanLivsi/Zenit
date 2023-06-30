//
// Created by os on 8/12/22.
//

#ifndef OS_PROJEKAT_TESTOVI_SYSCALL_CPP_H
#define OS_PROJEKAT_TESTOVI_SYSCALL_CPP_H
#include "syscall_c.h"
#include "riscv.h"
extern bool CPPAPI;

void* operator new (size_t);
void operator delete (void*);
class Thread {
public:
    friend class PeriodicThread;
    friend class TCB;
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t t);
protected:

    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore () {
        sem_close(myHandle);
    }
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};
class PeriodicThread : public Thread {
public:
    void stopThread();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
};
class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif //OS_PROJEKAT_TESTOVI_SYSCALL_CPP_H
