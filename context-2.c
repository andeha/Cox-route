/* context-2.c | makecontext and swapcontext replacements originally from libtask. */

#include "snapshot.h"
#include <stdarg.h>
#include <string.h>

void makecontext(struct ucontext *ucp, void (*func)(void), int argc, ...)
{
   long *sp;
   va_list va;
   memset(&ucp->uc_mcontext, 0, sizeof ucp->uc_mcontext);
   /* if(argc != 2) *(int*)0 = 0; */
   va_start(va, argc);
   ucp->uc_mcontext.mc_rdi = (long)va_arg(va, void *);
   /* ucp->uc_mcontext.mc_rsi = va_arg(va, int); */
   va_end(va);
   sp = (long*)ucp->uc_stack.ss_sp+ucp->uc_stack.ss_size/sizeof(long);
   sp -= argc;
   sp = (void*)((uintptr_t)sp - (uintptr_t)sp%16);	/* 16-align for OS X */
   *--sp = 0; /* return address */
   ucp->uc_mcontext.mc_rip = (long)func;
   ucp->uc_mcontext.mc_rsp = (long)sp;
}

int swapcontext(struct ucontext *oucp, const struct ucontext *ucp)
{
   if (getcontext(oucp) == 0) setcontext(ucp);
   return 0;
}

