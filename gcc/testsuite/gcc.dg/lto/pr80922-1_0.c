/* PR middle-end/80922 */

/* { dg-lto-do link } */
/* { dg-lto-additional-options "-O2 -Wfree-nonheap-object" } */

void myfree (void *ptr1, void *ptr2, void *ptr3, void *ptr4)
{
  __builtin_free (ptr1); /* { dg-lto-warning "-Wfree-nonheap-object" } */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wfree-nonheap-object"
  __builtin_free (ptr2);
  #pragma GCC diagnostic pop
  __builtin_free (ptr3); /* { dg-lto-warning "-Wfree-nonheap-object" } */
  _Pragma("GCC diagnostic push");
  _Pragma("GCC diagnostic ignored \"-Wfree-nonheap-object\"");
  #pragma GCC diagnostic push
  __builtin_free (ptr4);
}

/* Since these two functions are in the same translation unit, the
   diagnostic pragmas applied above are still in force.  */
void myfree2 (void *ptr1, void *ptr2, void *ptr3)
{
  __builtin_free (ptr1);
  #pragma GCC diagnostic pop
  __builtin_free (ptr2);
  _Pragma("GCC diagnostic pop");
  __builtin_free (ptr3); /* { dg-lto-warning "-Wfree-nonheap-object" } */
}
