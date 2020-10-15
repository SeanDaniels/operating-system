/* test_and_set.S - test and set (for x86) */
	.text
    .globl test_and_set

/*------------------------------------------------------------------------
 * test and set -  
 *------------------------------------------------------------------------*/
    ;; /* eax used as return register */
    ;; pushl %ebp /* store current ebp value onto stack */

    ;; movl %esp, %ebp // save current stack pointer into ebp register
    ;; pushl %edx
    ;; pushl %ebx

    ;; movl 8(%ebp), %eax // ptr -> get arg 1 (pointer to old_value) that was passed, store into ecx
    ;; movl (%eax), %eax // old_value = *ptr -> get content from pointer
    ;; movl %eax, -4(%ebp)
    ;; movl 8(%ebp), %eax // new_value -> get arg 2 (new_val) that was passed, store into eax
    ;; movl 12(%ebp), %edx // new_value -> get arg 2 (new_val) that was passed, store into eax
    ;; movl %edx, (%eax)
    ;; ;; xchg 8(%ebp), %eax  //put new_value into old value


    ;; popl %edx
    ;; popl %ebp
    ;; ret
    ;;
test_and_set:
    pushl	%ebp
    movl	%esp, %ebp
    pushl %edx
    // i want *ptr to equal new value
    movl	8(%ebp), %eax // ptr to old_value
    movl	(%eax), %eax // return old_value, old_value = *ptr -> save content of old value to return register
    movl	%eax, -4(%ebp) // store value of *ptr in this functions frame
    movl	8(%ebp), %eax // get ptr to old value again
    movl	12(%ebp), %edx // get new value
    xchg    %edx, (%eax) // store new value at address indicated by old_value
    movl	-4(%ebp), %eax // load old_value (in this functions frame) into return register

    pop %edx
    pop %ebp
    ret
