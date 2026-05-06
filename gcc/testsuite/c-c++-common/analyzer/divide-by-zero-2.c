/* { dg-additional-options "-fno-analyzer-state-merge" } */

extern int
get_value (void);

int
test (int flag)
{
  int x = 42;
  int y = 0; /* { dg-message "value originates here" } */
  if (flag)
    y = get_value ();
  return x / y; /* { dg-warning "division by zero" } */
}
