// { dg-do run { target c++17 } }
// { dg-options "-D__STDCPP_WANT_MATH_SPEC_FUNCS__" }

// 8.1.13 ellint_3 - non-finite phi must not produce UB

#include <cmath>
#include <limits>
#include <stdexcept>
#include <testsuite_hooks.h>

void
test01()
{
  bool caught = false;
  try
    {
      (void) std::ellint_3f(0.5F, 0.5F, std::numeric_limits<float>::infinity());
    }
  catch (const std::domain_error&)
    {
      caught = true;
    }
  VERIFY(caught);
}

void
test02()
{
  bool caught = false;
  try
    {
      (void) std::ellint_3(0.5, 0.5, std::numeric_limits<double>::infinity());
    }
  catch (const std::domain_error&)
    {
      caught = true;
    }
  VERIFY(caught);
}

void
test03()
{
  bool caught = false;
  try
    {
      (void) std::ellint_3l(0.5L, 0.5L,
			    std::numeric_limits<long double>::infinity());
    }
  catch (const std::domain_error&)
    {
      caught = true;
    }
  VERIFY(caught);
}

int
main()
{
  test01();
  test02();
  test03();
  return 0;
}
