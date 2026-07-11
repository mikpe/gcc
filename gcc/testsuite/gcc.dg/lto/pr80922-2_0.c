/* PR middle-end/80922 */
/* This is similar to pr80922-1, but myfree() and myfree2() are split into
   separate objects, so that they use two different line maps.  */

/* { dg-lto-do incr-link } */
/* { dg-lto-additional-options "-O2 -Wfree-nonheap-object" } */

void myfree (void *ptr1, void *ptr2, void *ptr3, void *ptr4)
{
  __builtin_free (ptr1); /* { dg-lto-warning "-Wfree-nonheap-object" } */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wfree-nonheap-object"
  __builtin_free (ptr2);
  #pragma GCC diagnostic pop
  __builtin_free (ptr3); /* { dg-lto-warning "-Wfree-nonheap-object" } */
  _Pragma("GCC diagnostic ignored \"-Wfree-nonheap-object\"");
  __builtin_free (ptr4);
}
