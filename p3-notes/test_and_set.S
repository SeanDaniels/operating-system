/* test_and_set.S - test and set (for x86) */
	.text
    .globl test_and_set

/*------------------------------------------------------------------------
 * test and set -  
 *------------------------------------------------------------------------
 */
test_and_set:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl 8(%ebp), %ebx
    xchg %eax, %ebx
    movl %ebp, %esp
    popl %ebp
    ret
