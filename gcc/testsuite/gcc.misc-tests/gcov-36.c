/* { dg-options "-fcondition-coverage -ftest-coverage" } */
/* { dg-do run } */

/* Some side effect to stop branches from being pruned.  */
int x = 0;

void
zero_elision_single (int a, int b, int c, int d)
{
    if (a || b || c || d) /* conditions(5/8) true(1 2 3) */
			   /* conditions(end) */
	x = 1;
    else
	x = 2;
}

int main ()
{
    zero_elision_single (1, 0, 0, 0);
    zero_elision_single (0, 0, 0, 0);
}

/* { dg-final { run-gcov conditions { --conditions gcov-36.c } } } */
