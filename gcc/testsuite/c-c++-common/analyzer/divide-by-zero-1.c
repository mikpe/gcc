#include "analyzer-decls.h"

static int __attribute__((noipa))
return_zero (void)
{
  return 0; /* { dg-message "value originates here" } */
}

void
test_div (int a)
{
  __analyzer_eval (a / return_zero () == 0); /* { dg-warning "division by zero \\\[-Wanalyzer-div-by-zero\\\]" } */
}

void
test_mod (int a)
{
  __analyzer_eval (a % return_zero () == 0); /* { dg-warning "division by zero \\\[-Wanalyzer-div-by-zero\\\]" } */
}
