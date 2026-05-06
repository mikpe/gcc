/* { dg-additional-options "-fno-analyzer-state-merge" } */

extern int
get_value (void);

int
test (int flag, int flag_2, int flag_3)
{
  int x = 42; /* { dg-bogus "value originates here" } */
  int y = 10; /* { dg-bogus "value originates here" } */
  int z = 0; /* { dg-message "value originates here" } */
  if (flag)
    y = get_value ();
  if (flag_2)
    z = get_value ();
  if (flag_3)
    y = z;
  return x / y; /* { dg-warning "division by zero" } */
}
