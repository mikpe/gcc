/* PR target/126283 */
/* { dg-do compile } */
/* { dg-options "-O2 -masm=att" } */
/* { dg-additional-options "-mregparm=1" { target ia32 } } */
/* { dg-final { scan-assembler-not "movl\[\\t \]+" } } */

int f (short x) {
  return __builtin_bswap16(x);
}
