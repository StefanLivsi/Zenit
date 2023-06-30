//
// Created by os on 8/16/22.
//

#ifndef OS_PROJEKAT_TESTOVI_CONSOLE_H
#define OS_PROJEKAT_TESTOVI_CONSOLE_H
#include "../h/myBuffer.h"
#include "../h/MemoryAllocator.h"
#include "../h/KernelObject.hpp"
class myConsole{
public:

    friend class Riscv;
    static void initialize();
    //static char getc ();
    //static void putc (char);
    static myBuffer *outBuf;
    static myBuffer *inBuf;
};
#endif //OS_PROJEKAT_TESTOVI_CONSOLE_H
