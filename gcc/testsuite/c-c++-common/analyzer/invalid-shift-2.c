unsigned char
do_shift (unsigned char val, int bits) /* { dg-message "34: entry to 'do_shift' with problematic value for 'bits'" } */
{
  return val << bits; /* { dg-warning "Wanalyzer-shift-count-overflow" } */
}

int
test (unsigned char ch)
{
  return do_shift (ch, 1000); /* { dg-message "passing problematic value from 'test' to 'do_shift' via parameter 2" } */
}
