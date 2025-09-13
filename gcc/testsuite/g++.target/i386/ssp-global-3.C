/* { dg-do compile { target fstack_protector } } */
/* { dg-options "-O2 -fstack-protector-all -mstack-protector-guard=global" } */

extern char __stack_chk_guard; /* { dg-error "conflicting declaration 'char __stack_chk_guard'" } */
