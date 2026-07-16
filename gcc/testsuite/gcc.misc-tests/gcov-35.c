/* { dg-options "-fcondition-coverage -ftest-coverage -fprofile-update=atomic" } */
/* { dg-do run } */
/* { dg-require-effective-target profile_update_atomic } */

/* Some side effect to stop branches from being pruned.  */
int x = 0;

void
zero_elision_atomic (int a, int b, int c, int d)
{
    if (a || b || c || d) /* conditions(5/8) true(1 2 3) */
			   /* conditions(end) */
	x = 1;
    else
	x = 2;
}

int main ()
{
    zero_elision_atomic (1, 0, 0, 0);
    zero_elision_atomic (0, 0, 0, 0);
}

/* { dg-final { run-gcov conditions { --conditions gcov-35.c } } } */
