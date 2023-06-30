#include "../h/myBuffer.h"
#include "../h/Semaphore.h"

myBuffer::myBuffer(int _cap) : cap(_cap + 1), head(0), tail(0) {
    //size_t newsize = (cap*sizeof(int)+sizeof(FreeMem))/MEM_BLOCK_SIZE;
    //if(((cap*sizeof(int)+sizeof(FreeMem))) % MEM_BLOCK_SIZE != 0) newsize++;
    //buffer = (int*)MemoryAllocator::mem_alloc(cap*sizeof(int));
    buffer = (int*)kmalloc(cap*sizeof (int));
    /*itemAvailable = new mySemaphore(0);
    spaceAvailable = new mySemaphore(_cap);
    mutexHead = new mySemaphore(1);
    mutexTail = new mySemaphore(1);*/
    itemAvailable = mySemaphore::createSemaphore(0);
    spaceAvailable = mySemaphore::createSemaphore(_cap);
    mutexHead = mySemaphore::createSemaphore(1);
    mutexTail = mySemaphore::createSemaphore(1);
}

myBuffer::~myBuffer() {
    while (getCnt() > 0) {
        char ch = buffer[head];
        putc(ch);
        head = (head + 1) % cap;
    }
    kfree(buffer);
    delete itemAvailable;
    delete spaceAvailable;
    delete mutexTail;
    delete mutexHead;
}

void myBuffer::put(char val) {
    spaceAvailable->wait();
    mutexTail->wait();

    buffer[tail] = val;
    tail = (tail + 1) % cap;

    mutexTail->signal();
    itemAvailable->signal();

}

char myBuffer::get() {
    itemAvailable->wait();
    mutexHead->wait();

    char ret = buffer[head];
    head = (head + 1) % cap;

    mutexHead->signal();
    spaceAvailable->signal();

    return ret;
}

int myBuffer::getCnt() {
    int ret;

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

    return ret;
}

myBuffer *myBuffer::createBuffer(int _cap) {
    myBuffer* buf = (myBuffer*)kmem_cache_alloc(SlabAllocator::cache_myBuffer);
    buf->init(_cap);
    return buf;
}

void myBuffer::init(int _cap) {
    cap = _cap+1;
    head = 0;
    tail = 0;
    buffer = (int*)SlabAllocator::kmalloc(cap*sizeof (int));

    itemAvailable = mySemaphore::createSemaphore(0);
    spaceAvailable = mySemaphore::createSemaphore(_cap);
    mutexHead = mySemaphore::createSemaphore(1);
    mutexTail = mySemaphore::createSemaphore(1);
}
