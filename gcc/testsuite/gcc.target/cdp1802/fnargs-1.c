/* { dg-do run } */
/* { do-options "-Os -fno-omit-frame-pointer" } */

/* Test setup of and access to stacked parameters.  */

short e = 5;

void __attribute__((noinline,noclone,noipa))
f (short r7, short r8, short r9, short r10, short x)
{
    if (x != e)
	__builtin_abort ();
}

int main (void)
{
    f (1, 2, 3, 4, e);
    return 0;
}
