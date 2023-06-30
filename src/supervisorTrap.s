# 1 "src/supervisorTrap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/supervisorTrap.S"
.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN3TCB10swapSpSsp1Ev
.extern _ZN3TCB10swapSpSsp2Ev

.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
    # push all registers to stack

    csrw sscratch, ra
    #call _ZN3TCB10swapSpSsp1Ev
    csrr ra, sscratch

    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv20handleSupervisorTrapEv

    # pop all registers from stack
    csrw sscratch, a0
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
    csrr a0, sscratch

    csrw sscratch, ra
    #call _ZN3TCB10swapSpSsp2Ev
    csrr ra, sscratch


    sret
