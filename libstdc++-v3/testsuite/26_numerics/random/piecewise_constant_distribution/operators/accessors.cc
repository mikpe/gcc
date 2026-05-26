// { dg-do run { target c++11 } }
// { dg-require-cstdint "" }

#include <random>
#include <testsuite_hooks.h>

template<typename RealType>
void
test_exact()
{
  RealType x[6] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
  RealType wt[5] = {0.5, 1.0, 3.5, 2.5, 0.5};

  std::piecewise_constant_distribution<RealType>
    u(std::begin(x), std::end(x), wt);

  const std::vector<RealType>& interval = u.intervals();
  VERIFY( interval.size() == 6 );
  VERIFY( interval[0] == RealType(0.0) );
  VERIFY( interval[5] == RealType(5.0) );

#ifdef _GLIBCXX_USE_OLD_PIECEWISE_DISTRIBUTIONS
  const std::vector<double>& density = u.densities();
#else
  const std::vector<RealType>& density = u.densities();
#endif

  VERIFY( density.size() == 5 );
  VERIFY( density[0] == RealType(0.5 / 8.0) );
  VERIFY( density[1] == RealType(1.0 / 8.0) );
  VERIFY( density[2] == RealType(3.5 / 8.0) );
  VERIFY( density[3] == RealType(2.5 / 8.0) );
  VERIFY( density[4] == RealType(0.5 / 8.0) );
}

template<typename RealType>
void
test_precision_depended()
{
  constexpr bool preserved
#ifdef _GLIBCXX_USE_RESULT_TYPE_FOR_PIECEWISE_DENSITIES
    = true;
#else
    = sizeof(RealType) <= sizeof(double);
#endif

  RealType x[3]{0.0, 0.5, 1.0};
  constexpr RealType step
    = std::numeric_limits<RealType>::epsilon();
  RealType wt[2]{RealType(1) - step, RealType(1) + step};

  std::piecewise_constant_distribution<RealType>
    u(std::begin(x), std::end(x), wt);

  const std::vector<RealType>& interval = u.intervals();
  VERIFY( interval.size() == 3 );
  VERIFY( interval[0] == RealType(0.0) );
  VERIFY( interval[2] == RealType(1.0) );

#ifdef _GLIBCXX_USE_OLD_PIECEWISE_DISTRIBUTIONS
  const std::vector<double>& density = u.densities();
#else
  const std::vector<RealType>& density = u.densities();
#endif

  VERIFY( density.size() == 2 );
  VERIFY( density[0] == (preserved ? wt[0] : RealType(1)) );
  VERIFY( density[1] == (preserved ? wt[1] : RealType(1)) );
}

int main()
{
  using namespace __gnu_test;
  test_exact<float>();
  test_exact<double>();
  test_exact<long double>();

  test_precision_depended<float>();
  test_precision_depended<double>();
  test_precision_depended<long double>();

  return 0;
}
