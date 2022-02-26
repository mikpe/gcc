/* This is present merely to alleviate link errors when memmove and memcpy are
   defined in the same C library object file.  */

extern void abort (void);
extern int inside_main;

__attribute__((noinline))
void *
memcpy (void *dest, const void *src, __SIZE_TYPE__ len)
{
  char *cdest = dest;
  const char *csrc = src;
  __SIZE_TYPE__ i;

#ifdef __OPTIMIZE__
  if (inside_main)
    abort ();
#endif

  for (i = 0; i < len; i++)
    cdest[i] = csrc[i];

  return dest;
}
