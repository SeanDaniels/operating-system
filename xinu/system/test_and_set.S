/* test_and_set.S - test and set (for x86) */
	.text
    .globl test_and_set

/*------------------------------------------------------------------------
 * test and set -  
 *------------------------------------------------------------------------*/


test_and_set:
    // eax used as return register
    pushl %ebp // store current ebp value onto stack
    //() -> indirection: get value at memory location %x
    movl %esp, %ebp // save current stack pointer into ebp register
    //%ebp is a register, (%ebp) is the content of the register, x(%ebp) is an x offset from the content of the register
    movl 8(%ebp), %edx // get arg 1 that was passed, store into ecx
    movl 12(%ebp), %eax // get arg 2 that was passed, store into eax

    addl %edx, %eax // add contents of eax and ebx, store into ebx
    popl %ebp
    ret
