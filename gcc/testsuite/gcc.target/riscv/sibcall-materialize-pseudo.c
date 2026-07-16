/* { dg-do compile { target { rv64 } } } */
/* { dg-options "-O2 -fPIC -fdump-rtl-expand" } */

extern int callee_value (int) __attribute__ ((weak));
extern void callee_void (int) __attribute__ ((weak));

int
caller_value (int x)
{
  return callee_value (x);
}

void
caller_void (int x)
{
  callee_void (x);
}

/* { dg-final { scan-rtl-dump-times {call_insn/j} 2 "expand" } } */
/* { dg-final { scan-rtl-dump-not {reg:DI 6 t1} "expand" } } */
