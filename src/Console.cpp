//
// Created by os on 8/16/22.
//
#include "../h/Console.h"
#include "../h/Semaphore.h"
myBuffer* myConsole::inBuf;
myBuffer* myConsole::outBuf;
void myConsole::initialize() {
    /*outBuf = new myBuffer(1024);
    inBuf = new myBuffer(1024);*/
    outBuf = myBuffer::createBuffer(4096);
    inBuf = myBuffer::createBuffer(4096);

}
