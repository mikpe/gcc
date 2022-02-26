/* { dg-do compile } */
/* { dg-options "-O2" } */

void f1(int *a, int x){
  int i;
  for(i = 0; i < 512; i++){
    *a = x;
    a += 2;
  }
}

/* { dg-final { scan-assembler-times "memw\\(r\.\\+\\+#8\\) =" 1 } } */
