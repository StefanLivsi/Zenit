//
// Created by os on 5/19/22.
//

#ifndef OSPROJEKAT_LIST_H
#define OSPROJEKAT_LIST_H
#include "../h/MemoryAllocator.h"
#include "../h/KernelObject.hpp"
#include "../h/SlabAllocator.hpp"
//class TCB;

template <typename T> class List : public KernelObject{
private:
    struct Elem : public KernelObject
    {


        void operator delete(void *p) noexcept
        {
            kmem_cache_free(SlabAllocator::cache_Elem, p);
        }
        void operator delete[](void *p) noexcept
        {
            kmem_cache_free(SlabAllocator::cache_Elem, p);
        }
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
        Elem(){}
        public: static Elem* createElem(T *data, Elem* next){
            Elem* elem = (Elem*)kmem_cache_alloc(SlabAllocator::cache_Elem);
            elem->data = data;
            elem->next = next;
            return elem;
        }

    };

    Elem *head, *tail;

public:
    friend class SlabAllocator;

    List() : head(0), tail(0) {}

    List(const List &) = delete;

    List &operator=(const List &) = delete;

    int cnt = 0;

    void operator delete(void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_List, p);
    }
    void operator delete[](void *p) noexcept
    {
        kmem_cache_free(SlabAllocator::cache_List, p);
    }

    void addFirst(T *data)
    {
        //Elem *elem = new Elem(data, head);
        Elem *elem = Elem::createElem(data,head);
        head = elem;
        if (!tail) { tail = head; }
        cnt++;
    }

    void addLast(T *data)
    {
        //Elem *elem = new Elem(data, 0);
        Elem *elem = Elem::createElem(data,0);
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
        cnt++;
    }


    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        cnt--;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        cnt--;
        return ret;
    }
    T* remove(int pos){
        if(pos >= cnt) return nullptr;
        if(pos==0) return removeFirst();
        if(pos==(cnt-1)) return removeLast();
        Elem *prev = 0;
        Elem *curr = head;
        int i = pos;
        for (; curr && i > 0; curr = curr->next, i--)
        {
            prev = curr;
        }
        prev->next = curr->next;
        cnt--;
        T *ret = curr->data;
        return ret;

    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
    T *operator[](int i) {
        //if (i < 0 || i >= cnt) throw GNemaPoz();
        int k = 0;
        for (Elem *temp = head; temp; temp = temp->next) {
            if (k++ == i) return temp->data;
        }
        return nullptr;
    }

    const T *operator[](int i) const {
        return const_cast<List &>(*this)[i];
    }
};

#endif //OSPROJEKAT_LIST_H
