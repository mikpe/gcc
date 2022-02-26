/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-skip-if "conditional/new-value loads/stores need >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

void f4(char *p, char q[], int i){
  if(p){
    *p = q[i];
  }
}

/* { dg-final { scan-assembler-times "if \\(!p\.\\.new\\) r\. = memb\\(r\.\\+r\.<<#0\\)" 1 } } */
/* { dg-final { scan-assembler-times "if \\(!p\.\\.new\\) memb\\(r\.+#0\\) = r\.\\.new" 1 } } */
