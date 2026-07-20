/* { dg-do compile } */
/* { dg-additional-options "-fdump-tree-omplower" } */

/* Check that a single, non-OMPT variant of libgomp call is emitted for the
   scope construct, only with a task reduction clause.  */

int x;

void
f1 (void)
{
  #pragma omp scope
  ;
}

void
f2 (void)
{
  #pragma omp scope reduction(task, +:x)
  ;
}

/* { dg-final { scan-tree-dump-times "GOMP_scope_start \\(" 1 "omplower" } } */
/* { dg-final { scan-tree-dump-not "GOMP_scope_start_with_end" "omplower" } } */
/* { dg-final { scan-tree-dump-not "GOMP_scope_end" "omplower" } } */
