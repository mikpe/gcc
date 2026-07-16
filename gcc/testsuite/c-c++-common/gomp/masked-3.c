/* { dg-do compile } */
/* { dg-additional-options "-fopenmp-ompt -fdump-tree-omplower" } */

/* Check that OMPT variants of libgomp calls are emitted.  */

void bar (void);

void
foo (void)
{
  #pragma omp masked
  bar ();
}

/* { dg-final { scan-tree-dump-times "GOMP_has_masked_thread_num_with_end" 1 "omplower" } } */
/* { dg-final { scan-tree-dump-times "GOMP_masked_end" 1 "omplower" } } */
/* { dg-final { scan-tree-dump-not "omp_get_thread_num" "omplower" } } */
