/* $15 is call-saved, and a function that needs a frame pointer clobbers it.
   The VLA lives in a static helper that is inlined into the caller, so the
   caller acquires dynamic stack allocation without frame_pointer_needed
   being known when IRA runs; LRA only decides a frame pointer is needed
   later, while marking the elimination not possible.  The prologue must
   still save $15 -- the epilogue restores it unconditionally, and used to
   read it back from the return address slot.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */

#include <string.h>

extern void use (const char *, unsigned);
extern void sink (char);
extern unsigned Indent;

static void
ind (const char *s_, unsigned n)
{
  char a[n + 1];

  memcpy (a, s_, n + 1);
  for (unsigned i = 0; i < Indent; i++)
    sink (' ');
  use (a, n);
}

void
caller (unsigned name)
{
  ind ("", 0);
  use ("x", name);
}

/* If the frame pointer is set up from the stack pointer, $15 must have been
   saved first.  */
/* { dg-final { scan-assembler "stq\[ \t\]+\\\$15" } } */
