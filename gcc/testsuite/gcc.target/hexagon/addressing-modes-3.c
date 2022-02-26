/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-skip-if "absolute+scaled reg requires >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

char ga[1024];

char f2(int i){
  return ga[i];
}

char f3(int i){
  return ga[4 * i];
}

/* { dg-final { scan-assembler-times "memub\\(r\.<<#0\\+##ga\\)" 1 } } */
/* { dg-final { scan-assembler-times "memub\\(r\.<<#2\\+##ga\\)" 1 } } */
