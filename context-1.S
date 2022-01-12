/*  context-1.S | setcontext and getcontext replacements originally from libtask. */

#define START(name)
#define END(name)
    
#if defined __x86_64__
    
    .globl _setmcontext
    START(_setmcontext)
_setmcontext:
    movq    16(%rdi), %rsi
    movq    24(%rdi), %rdx
    movq    32(%rdi), %rcx
    movq    40(%rdi), %r8
    movq    48(%rdi), %r9
    movq    56(%rdi), %rax
    movq    64(%rdi), %rbx
    movq    72(%rdi), %rbp
    movq    80(%rdi), %r10
    movq    88(%rdi), %r11
    movq    96(%rdi), %r12
    movq   104(%rdi), %r13
    movq   112(%rdi), %r14
    movq   120(%rdi), %r15
    movq   184(%rdi), %rsp
    pushq  160(%rdi)             /* new %eip */
    movq     8(%rdi), %rdi
    ret
    END(_setmcontext)
    
    .globl _getmcontext
    START(_getmcontext)
_getmcontext:
    movq     %rdi, 8(%rdi)
    movq     %rsi, 16(%rdi)
    movq     %rdx, 24(%rdi)
    movq     %rcx, 32(%rdi)
    movq      %r8, 40(%rdi)
    movq      %r9, 48(%rdi)
    movq       $1, 56(%rdi)        /* %rax */
    movq     %rbx, 64(%rdi)
    movq     %rbp, 72(%rdi)
    movq     %r10, 80(%rdi)
    movq     %r11, 88(%rdi)
    movq     %r12, 96(%rdi)
    movq     %r13, 104(%rdi)
    movq     %r14, 112(%rdi)
    movq     %r15, 120(%rdi)
    
    movq   (%rsp), %rcx	/* %rip */
    movq     %rcx, 160(%rdi)
    leaq  8(%rsp), %rcx	/* %rsp */
    movq     %rcx, 184(%rdi)
    
    movq 32(%rdi), %rcx	/* restore %rcx */
    movq       $0, %rax
    ret
    END(_getmcontext)
    
#elif defined __armv8a__
    
    .globl _getmcontext
    START(_getmcontext)
_getmcontext:
    str    r1, [r0,#4]
    str    r2, [r0,#8]
    str    r3, [r0,#12]
    str    r4, [r0,#16]
    str    r5, [r0,#20]
    str    r6, [r0,#24]
    str    r7, [r0,#28]
    str    r8, [r0,#32]
    str    r9, [r0,#36]
    str   r10, [r0,#40]
    str   r11, [r0,#44]
    str   r12, [r0,#48]
    str   r13, [r0,#52]
    str   r14, [r0,#56]
    /* store 1 as r0-to-restore */
    mov    r1, #1
    str    r1, [r0]
    /* return 0 */
    mov    r0, #0
    mov    pc, lr
    END(_getmcontext)
     
    .globl _setmcontext
    START(_setmcontext)
_setmcontext:
    ldr    r1,  [r0,#4]
    ldr    r2,  [r0,#8]
    ldr    r3,  [r0,#12]
    ldr    r4,  [r0,#16]
    ldr    r5,  [r0,#20]
    ldr    r6,  [r0,#24]
    ldr    r7,  [r0,#28]
    ldr    r8,  [r0,#32]
    ldr    r9,  [r0,#36]
    ldr    r10, [r0,#40]
    ldr    r11, [r0,#44]
    ldr    r12, [r0,#48]
    ldr    r13, [r0,#52]
    ldr    r14, [r0,#56]
    ldr    r0,  [r0]
    mov    pc,  lr
    END(_setmcontext)
    
#elif defined __mips__
    
    .globl _getmcontext
    START(_getmcontext)
_getmcontext:
    sw	$4, 24($4)
    sw	$5, 28($4)
    sw	$6, 32($4)
    sw	$7, 36($4)
    
    sw	$16, 72($4)
    sw	$17, 76($4)
    sw	$18, 80($4)
    sw	$19, 84($4)
    sw	$20, 88($4)
    sw	$21, 92($4)
    sw	$22, 96($4)
    sw	$23, 100($4)
    
    sw	$28, 120($4)	/* gp */
    sw	$29, 124($4)	/* sp */
    sw	$30, 128($4)	/* fp */
    sw	$31, 132($4)	/* ra */
    
    xor	$2, $2, $2
    j	$31
    nop
    END(_getmcontext)
    
    .globl _setmcontext
    START(_setmcontext)
_setmcontext:
    lw  $16, 72($4)
    lw  $17, 76($4)
    lw  $18, 80($4)
    lw  $19, 84($4)
    lw  $20, 88($4)
    lw  $21, 92($4)
    lw  $22, 96($4)
    lw  $23, 100($4)
    
    lw  $28, 120($4)    /* gp */
    lw  $29, 124($4)    /* sp */
    lw  $30, 128($4)    /* fp */
    
    /* 
     *  If we set $31 directly and j $31, 
     *  we would loose the outer return address.
     *  Use a temporary register, then.
     */
    lw  $8, 132($4).    /* ra */
    
    /*  bug: not setting the pc causes a bus error */
    lw  $25, 132($4)    /* pc */
    
    lw  $5, 28($4)
    lw  $6, 32($4)
    lw  $7, 36($4)
    lw  $4, 24($4)
    
    j   $8
    nop
    END(_setmcontext)
    
#endif


