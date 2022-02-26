/* { dg-do compile } */
/* { dg-options "-O2" } */

void f5(int *a, int x){
  int i;
  for(i = 0; i < 512; i++){
    a[i] = x;
  }
}

/* { dg-final { scan-assembler-times "memw\\(r\.\\+\\+#4\\) =" 1 } } */
