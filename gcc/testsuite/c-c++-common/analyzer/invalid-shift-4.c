unsigned char
do_shift (unsigned char val, int bits) /* { dg-message "34: entry to 'do_shift' with problematic value from \\\(2\\\) for 'bits'" } */
{
  return val << bits; /* { dg-warning "Wanalyzer-shift-count-negative" } */
}

int
test (unsigned char ch)
{
  int bits = -1; /* { dg-message "\\\(2\\\) value originates here" } */
  return do_shift (ch, bits); /* { dg-message "\\\(3\\\) passing problematic value from \\\(2\\\) from 'test' to 'do_shift' via parameter 2" } */
}
