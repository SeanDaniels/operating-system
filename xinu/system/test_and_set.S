/* test_and_set.S - test and set (for x86) */
	.text
    .globl test_and_set

/*------------------------------------------------------------------------
 * test and set -  
 *------------------------------------------------------------------------
 */
test_and_set:
    pushl %ebp // store current ebp value onto stack
    movl %esp, %ebp // save current stack pointer into ebp register
    pushal
    movl 8(%ebp), %eax // get arg 1 that was passed, store into eax
    movl 12(%ebp), %ebx // get arg 2 that was passed, store into ebx

    add %ebx, %eax // add contents of eax and ebx, store into ebx

    movl (%eax), %esp

    popal
    movl 4(%esp), %ebp
    ret
