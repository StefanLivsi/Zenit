//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "Scheduler.h"
#include "../lib/console.h"
#include "../h/MemoryAllocator.h"
#include "syscall_cpp.hpp"
#include "riscv.h"
#include "KernelObject.hpp"
#include "../h/SlabAllocator.hpp"
typedef unsigned long time_t;
// Thread Control Block

//class Thread;


class TCB : public KernelObject{
public:
    ~TCB() {
        if(stack)kfree(stack) ;
        if(sstack)kfree(sstack);
    }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)();

    static TCB *createThread(Body body, void* arg = nullptr, int sMode = 0);

    static void yield();

    static TCB *running;

    void setWrapper(Thread* thr){
        myThread = thr;
        context.ra = (uint64) &threadWrapper2;
    }
    void setPeriodicwrapper(time_t t){
        periodicTime = t;
        periodicThreads.addLast(this);
    }

    static int initThread(thread_t , Body body, void* arg1, int sMode);
    static TCB *createThreadEmpty();

    TCB(){}

private:

    /*TCB(Body body, uint64 timeSlice, void* arg1, int sMode) :
            body(body),
            //stack(((body != nullptr) || CPPAPI) ? (uint64*)MemoryAllocator::mem_alloc(SIZE_ALLOC) : nullptr),
            //sstack(((body != nullptr) || CPPAPI)? (uint64*)MemoryAllocator::mem_alloc(SIZE_ALLOC): nullptr),
            stack(((body != nullptr) || CPPAPI) ? (uint64*)MemoryAllocator::mem_alloc_wrapper(SIZE_ALLOC) : nullptr),
            sstack(((body != nullptr) || CPPAPI)? (uint64*)kmalloc(SIZE_ALLOC): nullptr),

            mode(sMode),

            context({mode == 0 ? (uint64) &threadWrapper : (uint64) &superthreadWrapper,
                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0,
                     sstack != nullptr ? (uint64) &sstack[STACK_SIZE] : 0
                    }),

            timeSlice(timeSlice),
            finished(false),
            sleepTime(0),
            arg(arg1)


    {
        //if (((body != nullptr && !CPPAPI)) || !CPPAPI) { Scheduler::put(this); }
        if(!CPPAPI && (body!=nullptr)) { Scheduler::put(this); }
    }*/

    struct Context
    {
        uint64 ra;
        uint64 sp;
        uint64 ssp;
    };

    Body body;
    uint64 *stack;
    uint64 *sstack;
    int mode;
    Context context;
    uint64 timeSlice;
    bool finished;
    time_t sleepTime;
    Thread *myThread;
    void* arg;
    time_t periodicTime = 0;
   // unsigned long long timeAlive = 0;


    friend class Riscv;
    friend class mySemaphore;

    static void threadWrapper();
    static void threadWrapper2();
    static void superthreadWrapper();
    static List<TCB> periodicThreads;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void swapSpSsp1();
    static void swapSpSsp2();

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 4096;
    static uint64 constexpr TIME_SLICE = 2;
    //static size_t constexpr SIZE_ALLOC = ((STACK_SIZE * sizeof (uint64)+sizeof (FreeMem)) / MEM_BLOCK_SIZE)+1;
    static size_t constexpr SIZE_ALLOC = STACK_SIZE * sizeof (uint64);

    static int exitThread();
    static int sleepThread(time_t);

};




#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP