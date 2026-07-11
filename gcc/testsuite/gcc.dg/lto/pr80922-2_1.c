/* The diagnostic pragmas left active in pr80922-2_0.c should not be in
   force anymore.  */
void myfree2 (void *ptr1, void *ptr2, void *ptr3)
{
  __builtin_free (ptr1); /* { dg-lto-warning "-Wfree-nonheap-object" } */
  _Pragma("GCC diagnostic push")
  #pragma GCC diagnostic ignored "-Wfree-nonheap-object"
  __builtin_free (ptr2);
  _Pragma("GCC diagnostic pop");
  __builtin_free (ptr3); /* { dg-lto-warning "-Wfree-nonheap-object" } */
}
