/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details -fdump-tree-release_ssa" } */

/* PR tree-optimization/116823 */
/* The clobber on a should not get in the way of phiprop here even if
   this is undefined code. */
/* We should have MIN_EXPR early on then too. */

static inline
const int &c(const int &d, const int &e) {
  if (d < e)
    return d;
  return e;
}

int g(int i, struct f *ff)
{
  const int &a = c(i, 10);
  return a;
}
/* { dg-final { scan-tree-dump-times "Inserting PHI for result of load" 1 "phiprop1"} } */
/* { dg-final { scan-tree-dump-times "MIN_EXPR" 1 "release_ssa"} } */

