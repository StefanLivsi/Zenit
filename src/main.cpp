//
// Created by os on 5/10/22.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
#include "../h/riscv.h"
#include "../h/tcb.h"
#include "../h/Console.h"
#include "../h/Semaphore.h"
#include "../h/slab.h"

void userMain();

void userMain1();

void userMain2();

void idleThread(){
    while(true) TCB::yield();
}

void consoleThread1(){
    while ((*(uint8 *) CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT)) {
        char c = myConsole::outBuf->get();
        *(char *) CONSOLE_TX_DATA = c;
    }
}
bool myCheck = false;
void main(){


    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&Riscv::supervisorTrap));
    //MemoryAllocator::init((void*)HEAP_START_ADDR,(void*)HEAP_END_ADDR);
    MemoryAllocator::init((void*)((char*)HEAP_START_ADDR +(BLOCK_SIZE*BLOCK_SIZE)),(void*)HEAP_END_ADDR);

    kmem_init((void*)HEAP_START_ADDR,0);
    myConsole::initialize();
    SlabAllocator::printCaches();
    myCheck = true;
    TCB *threads[5];
    threads[0] = TCB::createThread(nullptr);
    TCB::running = threads[0];
    threads[1] = TCB::createThread(consoleThread1, nullptr, 1);
    //threads[2] = TCB::createThread(userMain, nullptr, 0);
    threads[2] = TCB::createThread(userMain1, nullptr, 0);
    //threads[2] = TCB::createThread(userMain2, nullptr, 1);

    idleThreadTCB = TCB::createThread(idleThread, nullptr, 0);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(threads[2]->isFinished()
            ))
    {
        TCB::yield();
    }
    TCB::yield();

    threads[1]->setFinished(true);
    Scheduler::get();
    for (auto &thread: threads)
    {
        //delete thread;
        if(thread!= nullptr)
        kmem_cache_free(SlabAllocator::cache_TCB,thread);
    }

}