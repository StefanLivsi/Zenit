//
// Created by os on 5/19/22.
//
#include "../h/riscv.h"
#include "../h/tcb.h"
#include "../lib/console.h"
#include "../h/MemoryAllocator.h"
#include "../h/print.h"
#include "../h/syscall_c.h"
#include "../h/Console.h"
#include "../h/Semaphore.h"

extern MemoryAllocator* m_allocator;

void Riscv::popSppSpie()
{
    ms_sstatus(BitMaskSstatus::SSTATUS_SPIE);
    mc_sstatus(BitMaskSstatus::SSTATUS_SPP);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scauseVar = r_scause();
    if(scauseVar == 0x8000000000000001UL){ // PREKID OD TAJMERA, ASINHRONA PROMENA
        int i = 0;
        while (i < Scheduler::sleepThreadList.cnt){
            TCB* tcb = Scheduler::sleepThreadList[i];
            if(tcb->sleepTime==0){
                Scheduler::getSleep(i);
                Scheduler::put(tcb);

            }
            else{
                tcb->sleepTime-=1;
                i++;
            }
        }
        TCB::timeSliceCounter++;
        if(TCB::timeSliceCounter > TCB::running->getTimeSlice()){
            volatile uint64 sepc = r_sepc();
            volatile uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sepc(sepc);
            w_sstatus(sstatus);
        }
        mc_sip(SIP_SSIP);

    }
    else if (scauseVar == 0x0000000000000008UL || scauseVar == 0x0000000000000009UL){ //ecall IZ KORISNICKOG ILI SIS REZIMA
        uint64 callId;
        __asm__ volatile("mv %0, a0" : "=r" (callId)); // moze i kroz arg funkcije

        switch (callId) {
            case 1: { // MEM ALLOC
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                size_t sizearg;
                __asm__ volatile("mv %0, a1" : "=r"(sizearg));
                void *retval = MemoryAllocator::mem_alloc(sizearg);
                __asm__ volatile("mv a0, %0" : : "r"(retval));
                break;
            }
            case 2: { //MEM FREE
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                void *addrarg;
                __asm__ volatile("mv %0, a1" : "=r"(addrarg));
                int retval = MemoryAllocator::mem_free(addrarg);
                __asm__ volatile("mv a0, %0" : : "r"(retval));
                break;
            }
            case 3: { // SINHRONA PROMENA/YIELD
                volatile uint64 sepc = r_sepc()+4;
                //w_sepc(76376);
                volatile uint64 sstatus = r_sstatus(); // promena
                TCB::timeSliceCounter = 0;
                //__putc('a');
                TCB::dispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case 7:{
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                uint64 retval = TCB::exitThread();
                __asm__ volatile("mv a0, %0" : : "r"(retval));
                break;
            }
            case 8:{
                volatile uint64 sepc = r_sepc()+4;
                volatile uint64 sstatus = r_sstatus();
                TCB::timeSliceCounter = 0;
                time_t time;
                __asm__ volatile("mv %0, a1" : "=r"(time));
                uint64 retval = TCB::sleepThread(time);
                w_sepc(sepc);
                w_sstatus(sstatus);
                __asm__ volatile("mv a0, %0" : : "r"(retval));
                break;
            }
            case 9:{
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                args2* argss;
                __asm__ volatile("mv %0, a1" : "=r"(argss));

                *argss->t= TCB::createThread(argss->start_routine, argss->arg3);
                CPPAPI = false;

                break;
            }
            case 10:{
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                args*argss;
                __asm__ volatile("mv %0, a1" : "=r"(argss));
                *argss->arg1 = mySemaphore::createSemaphore(argss->arg2);

                break;
            }
            case 11:{
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                sem_t id;
                __asm__ volatile("mv %0, a1" : "=r"(id));
                while(id->value() < 0) id->unblock();
                kmem_cache_free(SlabAllocator::cache_mySemaphore,id);
                break;
            }
            case 12:{
                volatile uint64 sepc = r_sepc()+4;
                volatile uint64 sstatus = r_sstatus();
                sem_t id;
                __asm__ volatile("mv %0, a1" : "=r"(id));
                id->wait();
                w_sepc(sepc);
                w_sstatus(sstatus);
                break;
            }
            case 13:{
                volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
                sem_t id;
                __asm__ volatile("mv %0, a1" : "=r"(id));
                id->signal();
                break;
            }
            case 14:{
                volatile uint64 sepc = r_sepc()+4;
                volatile uint64 sstatus = r_sstatus();
                char c = myConsole::inBuf->get();
                w_sepc(sepc);
                w_sstatus(sstatus);
                __asm__ volatile("mv a0, %0" : : "r"(c));
                break;
            }
            case 15:{
                volatile uint64 sepc = r_sepc()+4;
                volatile uint64 sstatus = r_sstatus();
                char c;
                __asm__ volatile("mv %0, a1" : "=r"(c));
                myConsole::outBuf->put(c);
                w_sepc(sepc);
                w_sstatus(sstatus);
                break;
            }
            default:
            {   volatile uint64 sepc = r_sepc() + 4;
                w_sepc(sepc);
            }
        }
    }
    else if(scauseVar == 0x8000000000000009UL){

        int a = plic_claim();
        if(a==CONSOLE_IRQ) {
            while(*(uint8*)CONSOLE_STATUS & CONSOLE_RX_STATUS_BIT){
                char c = *(char*)CONSOLE_RX_DATA;
                //if(myConsole::inBuf->getCnt()<1000)
                myConsole::inBuf->put(c);
            }
            plic_complete(CONSOLE_IRQ);
        }

    }
    else
    {
       oldprintString("scause:");
        oldprintInteger(scauseVar);
        oldprintString("\nstval:");
        oldprintInteger(r_stval());
        oldprintString("\nsepc:");
        oldprintInteger(r_sepc());
        while(1);
    }
}
