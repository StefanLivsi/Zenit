//
// Created by os on 5/10/22.
//
#include "../h/MemoryAllocator.h"
#include "../h/riscv.h"
void* MemoryAllocator::MEM_SPACE_END;
void* MemoryAllocator::MEM_SPACE_BEGIN;
MemoryAllocator::FreeMem* MemoryAllocator::fmem_head;
MemoryAllocator::FreeMem MemoryAllocator::mymem;
void* MemoryAllocator::mem_alloc(size_t size) {
    //
    //size_t size = (oldsize+sizeof(FreeMem))/MEM_BLOCK_SIZE;
    //size++;
    //
    FreeMem* fm = fmem_head, *prev = nullptr;
    for(; fm!= nullptr; prev = fm, fm=fm->next)
        if(fm->size>=size) break;

    if(!fm) return nullptr;
    size_t remainingSize = fm->size - size;
    fm->size = size;
    if(remainingSize){
        FreeMem* newfm = (FreeMem*)((char*)fm + size*MEM_BLOCK_SIZE);
        newfm->size=remainingSize;
        newfm->next = fm->next;
        if(prev) prev->next = newfm;
        else fmem_head = newfm;
    }
    else{
        if(prev) prev->next = fm->next;
        else fmem_head = fm->next;
    }
    fm->next = nullptr;
    return (char*)fm + sizeof(FreeMem);
}

int MemoryAllocator::tryToJoin(FreeMem *fm) {
    if (!fm) return 0;
    FreeMem* test = (FreeMem*)((char*)fm+fm->size*MEM_BLOCK_SIZE);
    if (fm->next && test == (fm->next)) {
        fm->size += fm->next->size;
        fm->next = fm->next->next;
        return 1;
    } else{
        return 0;
    }

}

int MemoryAllocator::mem_free(void *addr) {
    if(addr < MEM_SPACE_BEGIN || addr >= MEM_SPACE_END) return -1;
    FreeMem* addr_aligned = (FreeMem*)((char*)addr - sizeof(FreeMem));
    if(((size_t)addr_aligned-(size_t)MEM_SPACE_BEGIN) % MEM_BLOCK_SIZE != 0) return -2;
    FreeMem* fm = fmem_head;
    if (!fmem_head || addr_aligned < fmem_head) {
        fm = nullptr;
    }
    else {
        for (; fm; fm = fm->next) {
            FreeMem *above = fm->next ? fm->next : (FreeMem *) (MEM_SPACE_END);
            if (addr_aligned > fm && addr_aligned < above) break;
        }
    }
    FreeMem* newfm = addr_aligned;
    if (fm) newfm->next = fm->next;
    else newfm->next = fmem_head;
    if(fm) fm->next = newfm;
    else fmem_head = newfm;
    tryToJoin(newfm);
    tryToJoin(fm);
    return 0;
}

void MemoryAllocator::init(void *spaceBegin, void *spaceEnd) {
    fmem_head = (FreeMem*)spaceBegin;
    //size_t block_num = (size_t)spaceBegin/MEM_BLOCK_SIZE;
    //MEM_SPACE_BEGIN = (void*)((block_num+1)*MEM_BLOCK_SIZE);//TODO PORAVNATI NA MEMBLOCKSIZE
    MEM_SPACE_BEGIN = spaceBegin;
    MEM_SPACE_END = spaceEnd;
    fmem_head->size = ((size_t)MEM_SPACE_END-(size_t)MEM_SPACE_BEGIN)/MEM_BLOCK_SIZE;
}

void *MemoryAllocator::mem_alloc_wrapper(size_t size) {
    size_t newsize = (size+sizeof(FreeMem))/MEM_BLOCK_SIZE;
    if((size+sizeof(FreeMem)) % MEM_BLOCK_SIZE != 0) newsize++;
    return mem_alloc(newsize);
}


