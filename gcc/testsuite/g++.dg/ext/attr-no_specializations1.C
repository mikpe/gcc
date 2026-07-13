// PR c++/120635
// { dg-do compile { target c++11 } }

template <typename T, typename U>
struct [[clang::no_specializations]] A {};		// { dg-message "declared 'clang::no_specializations' here" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[clang::no_specializations]] int b = 1;		// { dg-message "declared 'clang::no_specializations' here" "" { target c++14 } }
#endif
template <typename T, typename U>
[[__clang__::__no_specializations__]] int foo () { return 0; }	// { dg-message "declared 'clang::no_specializations' here" }
template <typename T>
struct B {
  struct [[clang::no_specializations]] C {};		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] static int b;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] int foo () { return 0; }// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  template <typename U>
  class [[clang::no_specializations]] D {};
};
A <int, long> c;
#if __cpp_variable_templates >= 201304
int d = b <int, long>;
#endif
int e = foo <int, long> ();
template <typename T>
struct D {};
template <>
struct A <int, int> { int a; };				// { dg-error "'struct A<int, int>' cannot be specialized" }
template <typename T>
struct A <long, T> { long b; };				// { dg-error "'struct A<long int, T>' cannot be specialized" }
template <typename T>
struct A <D <T>, D <T>> { D<T> c; };			// { dg-error "'struct A<D<T>, D<T> >' cannot be specialized" }
#if __cpp_variable_templates >= 201304
template <>
int b <int, int> = 2;					// { dg-error "'b<int, int>' cannot be specialized" "" { target c++14 } }
template <typename T>
int b <long, T> = 3;					// { dg-error "'b<long int, T>' cannot be specialized" "" { target c++14 } }
template <typename T>
int b <D <T>, D <T>> = 4;				// { dg-error "'b<D<T>, D<T> >' cannot be specialized" "" { target c++14 } }
#endif
template <>
int foo <int, int> () { return 1; }			// { dg-error "'int foo\\\(\\\) \\\[with T = int; U = int\\\]' cannot be specialized" }
struct [[clang::no_specializations]] E {};		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T>
using F [[clang::no_specializations]] = T;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
[[clang::no_specializations]] int f = 1;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
[[clang::no_specializations]] int bar () { return 0; }	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T, typename U>
struct G {};
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
int g = 1;
#endif
template <typename T, typename U>
int baz () { return 0; }
template <>
struct [[clang::no_specializations]] G <int, int> { int a; };		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T>
struct [[clang::no_specializations]] G <long, T> { long b; };		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T>
struct [[clang::no_specializations]] G <D <T>, D <T>> { D<T> c; };	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
#if __cpp_variable_templates >= 201304
template <>
[[clang::no_specializations]] int g <int, int> = 2;	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" "" { target c++14 } }
template <typename T>
[[clang::no_specializations]] int g <long, T> = 3;	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" "" { target c++14 } }
template <typename T>
[[clang::no_specializations]] int g <D <T>, D <T>> = 4;	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" "" { target c++14 } }
#endif
template <>
[[clang::no_specializations]] int baz <int, int> () { return 1; }	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T>
using H [[clang::no_specializations]] = int;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template struct A <double, double>;
extern template struct A <float, float>;
#if __cpp_variable_templates >= 201304
template int b <double, double>;
extern template int b <float, float>;
#endif
template int foo <double, double> ();
extern template int foo <float, float> ();
template <typename T>
struct D2 {};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-specialization"
template <>
struct A <short, int> { int a; };			// { dg-bogus "'struct A<int, int>' cannot be specialized" }
template <typename T>
struct A <long long, T> { long b; };			// { dg-bogus "'struct A<long int, T>' cannot be specialized" }
template <typename T>
struct A <D2 <T>, D2 <T>> { D2<T> c; };			// { dg-bogus "'struct A<D<T>, D<T> >' cannot be specialized" }
#if __cpp_variable_templates >= 201304
template <>
constexpr int b <short, int> = 2;			// { dg-bogus "'b<short, int>' cannot be specialized" }
template <typename T>
constexpr int b <long long, T> = 3;			// { dg-bogus "'b<long long int, T>' cannot be specialized" }
template <typename T>
constexpr int b <D2 <T>, D2 <T>> = 4;			// { dg-bogus "'b<D2<T>, D2<T> >' cannot be specialized" }
#endif
template <>
int foo <short, int> () { return 1; }			// { dg-bogus "'int foo\\\(\\\) \\\[with T = short; U = int\\\]' cannot be specialized" }
#pragma GCC diagnostic pop
