/* Verify that alloca storage is sufficiently aligned.  */
/* ??? May fail if BIGGEST_ALIGNMENT > STACK_BOUNDARY.  Which, I guess
   can only happen on !STRICT_ALIGNMENT targets.  */

typedef __SIZE_TYPE__ size_t;

struct dummy { int x __attribute__((aligned)); };
#define BIGGEST_ALIGNMENT __alignof__(struct dummy)

_Bool foo(void)
{
  char *p = __builtin_alloca(32);

  /* This test is making an assumption that pointers are integers that
     reference byte addressable memory in the obvious manner.
     This is wrong for the PDP10, where interpreting a pointer as an
     integer and incrementing it by one actually increments the memory
     reference by 4.
   */
#ifdef __PDP10__
  return ((size_t)p & (BIGGEST_ALIGNMENT/4 - 1)) == 0;
#else
  return ((size_t)p & (BIGGEST_ALIGNMENT - 1)) == 0;
#endif
}

int main()
{
  if (!foo())
    abort ();
  return 0;
}
