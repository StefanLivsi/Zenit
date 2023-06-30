//
// Created by os on 5/19/22.
//

#ifndef OSPROJEKAT_SCHEDULER_H
#define OSPROJEKAT_SCHEDULER_H
#include "List.h"
class TCB;
extern TCB* idleThreadTCB;
typedef TCB* thread_t;
class Scheduler{
public:


    static void put(TCB*);
    static TCB* get();
    static void putSleep(TCB*);
    static void init();
    static TCB* getSleep(int);
    static List<TCB> readyThreadQueue;
    static List<TCB> sleepThreadList;
private:

};
#endif //OSPROJEKAT_SCHEDULER_H
