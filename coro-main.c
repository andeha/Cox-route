/*  main.c | coroutines and the c language. */

#define false 0
#define true (! false)

#include "snapshot.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct __coro_t coro_t;
typedef int (*coro_function_t)(coro_t * coro);

typedef struct __coro_t {
  coro_function_t function; /* actual co-routine function. */
  struct ucontext base_context; /* stores context previous to coroutine jump. */
  struct ucontext overlay_context; /* stores coroutine context. */
  int yield_value; /* coroutine return/yield value. */
  int is_coro_finished; /* to indicate the current coroutine status. */
} coro_t;

coro_t * coro_await(coro_function_t function);
int coro_resume(coro_t * coro);
void coro_feedback(coro_t * coro, int value);
void coro_free(coro_t * coro);

void __coroutine_entry_point(coro_t * coro)
{
   int return_value = coro->function(coro);
   coro->is_coro_finished = true;
   coro_feedback(coro,return_value);
}

coro_t * coro_await(coro_function_t function)
{ int y;
   coro_t * coro = malloc(sizeof(struct __coro_t));
   getcontext(&coro->overlay_context);
   coro->is_coro_finished = false;
   coro->function = function;
   coro->overlay_context.uc_stack.ss_sp = malloc(8388608);
   coro->overlay_context.uc_stack.ss_size = 8388608; /* MINSIGSTKSZ alt. SIGSTKSZ */
   coro->overlay_context.uc_stack.ss_flags = 0;
   coro->overlay_context.uc_link = &coro->base_context;
   if (sigemptyset(&coro->overlay_context.uc_sigmask) < 0) { }
   makecontext(&coro->overlay_context, (void (*)())__coroutine_entry_point, 1, coro);
   swapcontext(&coro->base_context,&coro->overlay_context);
   puts("back-to-main");
   return coro;
}

int coro_resume(coro_t * coro)
{
   if (coro->is_coro_finished) return -1;
   swapcontext(&(coro->base_context),&(coro->overlay_context));
   return coro->yield_value;
}

void coro_feedback(coro_t * coro, int value)
{
   coro->yield_value = value;
   swapcontext(&coro->overlay_context,&coro->base_context);
}

void coro_free(coro_t * coro)
{
   free(coro->overlay_context.uc_stack.ss_sp);
   free(coro);
}

/* clang -o coroutines -g context-1.S context-2.c -DINCLUDE_MAIN coro-main.c. */

int corout₋helloworld(coro_t * coro)
{
   puts("Hello - part 1");
   coro_feedback(coro,1); /* ⬷ suspension point that returns the value `1`. */
   puts("World - part 2");
   return 2;
}

#if defined INCLUDE_MAIN
int
main(
  int argc, 
  const char * argv[]
)
{
   coro_t * coro = coro_await(corout₋helloworld);
   assert(coro->yield_value == 1); /* checking an already feedbacked value. */
   assert(coro_resume(coro) == 2); /* checking after restart returned value. */
   assert(coro_resume(coro) == -1); /* checking restarting finalized call. */
   coro_free(coro);
   puts("done");
   return 0;
}
#endif


