//
// Created by os on 5/19/22.
//
#include "../h/Scheduler.h"

List<TCB> Scheduler::readyThreadQueue;
List<TCB> Scheduler::sleepThreadList;
TCB* idleThreadTCB;

void Scheduler::put(TCB* t) {
    if(t == idleThreadTCB) return;
    readyThreadQueue.addLast(t);
}

TCB* Scheduler::get() {
    TCB* t = readyThreadQueue.removeFirst();
    if(!t) return idleThreadTCB;
    return t;
}

void Scheduler::putSleep(TCB *t) {
    sleepThreadList.addLast(t);
}

TCB *Scheduler::getSleep(int pos) {
    return sleepThreadList.remove(pos);
}

/*void Scheduler::init() {

}*/

