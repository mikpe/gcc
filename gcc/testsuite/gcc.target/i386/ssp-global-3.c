/* { dg-do compile { target fstack_protector } } */
/* { dg-options "-O2 -fstack-protector-all -mstack-protector-guard=global" } */

extern char *__stack_chk_guard; /* { dg-error "conflicting types for '__stack_chk_guard';" } */

char
foo (void)
{
  return *__stack_chk_guard;
}
