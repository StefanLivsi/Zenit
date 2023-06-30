//
// Created by os on 5/16/22.
//
#include "../h/syscall_c.h"
#include "../lib/mem.h"
#include "../lib/console.h"
#include "../h/riscv.h"
#include "../h/Semaphore.h"
#include "../h/tcb.h"
#include "../h/MemoryAllocator.h"

void *mem_alloc(size_t size) {
    size_t newsize = (size+sizeof(MemoryAllocator::FreeMem))/MEM_BLOCK_SIZE;
    if(size+sizeof(MemoryAllocator::FreeMem) % MEM_BLOCK_SIZE != 0) newsize++;
    __asm__ volatile("addi a0, x0, 1");
    __asm__ volatile("mv a1, %0" : : "r" (newsize));
    __asm__ volatile("ecall");
    void* retval;
    __asm__ volatile("mv %0, a0" : "=r" (retval));
    return retval;

}

int mem_free(void* arg) {
    __asm__ volatile("mv a1, %0" : : "r" (arg));
    __asm__ volatile("addi a0, x0, 2");
    __asm__ volatile("ecall");
    int retval;
    __asm__ volatile("mv %0, a0" : "=r" (retval));
    return retval;

}

void __printString(const char *string) {
    __asm__ volatile("mv a1, %0" : : "r" (string));
    __asm__ volatile("addi a0, x0, 5");
    __asm__ volatile("ecall");
}

void __printInteger(uint64 integer) {
    __asm__ volatile("mv a1, %0" : : "r" (integer));
    __asm__ volatile("addi a0, x0, 6");
    __asm__ volatile("ecall");
}

void thread_create(thread_t* handle, // TCB**
                   void(*start_routine)(void*),
                   void* arg){
    args2* argss = new args2{handle, reinterpret_cast<void (*)(void)>(start_routine), arg};
    __asm__ volatile("mv a1, %0" : : "r" (argss));
    __asm__ volatile("addi a0, x0, 9");
    __asm__ volatile ("ecall");
    CPPAPI = false;

}

void thread_dispatch(){
    __asm__ volatile("addi a0, x0, 3");
    __asm__ volatile ("ecall");
}

int thread_exit(){
    __asm__ volatile("addi a0, x0, 7");
    __asm__ volatile("ecall");
    int retval;
    __asm__ volatile("mv %0, a0" : "=r" (retval));
    return retval;
}

int time_sleep(time_t time){
    __asm__ volatile("mv a1, %0" : : "r" (time));
    __asm__ volatile("addi a0, x0, 8");
    __asm__ volatile("ecall");
    int retval;
    __asm__ volatile("mv %0, a0" : "=r" (retval));
    return retval;
}

char getc(){
    __asm__ volatile("addi a0, x0, 14");
    __asm__ volatile("ecall");
    char retval;
    __asm__ volatile("mv %0, a0" : "=r" (retval));
    return retval;

}

void putc(char c){
    __asm__ volatile("mv a1, %0" : : "r" (c));
    __asm__ volatile("addi a0, x0, 15");
    __asm__ volatile("ecall");
    //__putc('k');
}
int sem_open(sem_t* handle, unsigned init){ //**mySemaphore

    args *argss = new args{handle,init};
    __asm__ volatile("mv a1, %0" : : "r" (argss));
    __asm__ volatile("addi a0, x0, 10");
    __asm__ volatile ("ecall");

    //__asm__ volatile("mv %0, a0" : "=r" (*handle));
    if(!*handle) return -1;
    return 0;
}
int sem_close(sem_t handle){
    if(!handle) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("addi a0, x0, 11");
    __asm__ volatile("ecall");
    return 0;
}
int sem_wait(sem_t id){
    if(!id) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("addi a0, x0, 12");
    __asm__ volatile("ecall");
    if(!id) return -2;
    return 0;
}
int sem_signal(sem_t id){
    if(!id) return -1;
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("addi a0, x0, 13");
    __asm__ volatile("ecall");
    if(!id) return -2;
    return 0;
}
