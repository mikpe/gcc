/* { dg-do compile } */
/* { dg-options "-O1 -fno-pic -ftree-loop-distribute-patterns -fno-tree-dominator-opts -fno-tree-scev-cprop -march=x86-64" } */

/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "" "" { target { *-*-linux* && lp64 } } {^\t?\.} } } */

/*
**func:
**.LFB0:
**	.cfi_startproc
**	movl	\$0, %eax
**	.p2align 3
**.L2:
**	addl	\$1, %eax
**	cmpl	\$18, %eax
**	jne	.L2
**	cltq
**	movabsq	\$506381209866536711, %rdx
**	movq	%rdx, %xmm0
**	punpcklqdq	%xmm0, %xmm0
**	movups	%xmm0, buf\(%rax\)
**	movb	\$7, buf\+16\(%rax\)
**	ret
**	.cfi_endproc
**...
*/

extern char buf[512];

void
func (void)
{
  int i = 0;
  for (; i < 18; i++);
  for (; i < 35; i++) buf[i] = 7;
}
