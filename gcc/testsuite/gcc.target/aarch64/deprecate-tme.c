/* { dg-do compile } */

__attribute__((target("+tme")))
void a(void) {} /* { dg-warning {'\+tme' is deprecated} } */

__attribute__((target("+dotprod+tme")))
void b(void) {} /* { dg-warning {'\+tme' is deprecated} } */

#pragma GCC target ("+nothing+tme") /* { dg-warning {'\+tme' is deprecated} } */

void c(void) {}

/* No warning here, we only want to warn when tme transitions from
   disabled to enabled.  This helps keeps noise to a minimum.  */
__attribute__((target("+tme")))
void d(void) {}
