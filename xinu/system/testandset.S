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
    pushl %ebp
    movl %esp, %ebp
    pushl %edx

    movl 12(%ebp),%eax      /* eax = new_value                              */

    movl 8(%ebp), %edx      /* ecx = ptr  - THIS WAY, THE OLD VALUE POINTED by ptr is read into EAX during the exchange without any additional MOVL instruction needed */

    xchg (%edx),%eax        /* exchange eax with value at address ptr - THIS IS THE ATOMIC SWAP OPERATION       */

    pop %edx
    pop %ebp
    ret
