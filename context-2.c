/* context-2.c | makecontext and swapcontext replacements originally from libtask. */

#include "snapshot.h"

#if defined __x86_64__

void * memset(void *b, int c, size_t len);

void makecontext(struct ucontext *ucp, void (*func)(void), int argc, ...)
{
   long *sp;
   __builtin_va_list va;
   memset(&ucp->uc_mcontext, 0, sizeof ucp->uc_mcontext);
   /* if(argc != 2) *(int*)0 = 0; */
   __builtin_va_start(va, argc);
   ucp->uc_mcontext.mc_rdi = (long)__builtin_va_arg(va, void *);
   /* ucp->uc_mcontext.mc_rsi = va_arg(va, int); */
   __builtin_va_end(va);
   sp = (long*)ucp->uc_stack.ss_sp+ucp->uc_stack.ss_size/sizeof(long);
   sp -= argc;
   sp = (void*)((uintptr_t)sp - (uintptr_t)sp%16);	/* 16-align for OS X */
   *--sp = 0; /* return address */
   ucp->uc_mcontext.mc_rip = (long)func;
   ucp->uc_mcontext.mc_rsp = (long)sp;
}

#elif defined __mips__

void makecontext(struct ucontext *uc, void (*fn)(void), int argc, ...)
{
   int i, *sp;
   __builtin_va_list arg;
   
   __builtin_va_start(arg, argc);
   sp = (int*)uc->uc_stack.ss_sp+uc->uc_stack.ss_size/4;
   for(i=0; i<4 && i<argc; i++)
   	uc->uc_mcontext.mc_regs[i+4] = __builtin_va_arg(arg, int);
   __builtin_va_end(arg);
   uc->uc_mcontext.mc_regs[29] = (int)sp;
   uc->uc_mcontext.mc_regs[31] = (int)fn;
}

#elif defined __armv8a__

void makecontext(ucontext_t *uc, void (*fn)(void), int argc, ...)
{
   int i, *sp;
   va_list arg;
   
   sp = (int*)uc->uc_stack.ss_sp+uc->uc_stack.ss_size/4;
   __builtin_va_start(arg, argc);
   for(i=0; i<4 && i<argc; i++)
   	uc->uc_mcontext.gregs[i] = __builtin_va_arg(arg, uint);
   __builtin_va_end(arg);
   uc->uc_mcontext.gregs[13] = (uint)sp;
   uc->uc_mcontext.gregs[14] = (uint)fn;
}

#endif

int swapcontext(struct ucontext *oucp, const struct ucontext *ucp)
{
   if (getcontext(oucp) == 0) setcontext(ucp);
   return 0;
}

