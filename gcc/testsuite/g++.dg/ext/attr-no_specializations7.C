// PR c++/120635
// { dg-do compile { target c++11 } }
// { dg-options "-Wno-invalid-specialization" }

template <typename T, typename U>
class [[clang::no_specializations]] A {};		// { dg-bogus "declared 'clang::no_specializations' here" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[clang::no_specializations]] constexpr int b = 1;	// { dg-bogus "declared 'clang::no_specializations' here" }
#endif
template <typename T, typename U>
[[_Clang::__no_specializations__]] int foo () { return 0; }	// { dg-bogus "declared 'clang::no_specializations' here" }
template <typename T>
struct D {};
template <>
struct A <int, int> { int a; };				// { dg-bogus "'class A<int, int>' cannot be specialized" }
template <typename T>
struct A <long, T> { long b; };				// { dg-bogus "'class A<long int, T>' cannot be specialized" }
template <typename T>
struct A <D <T>, D <T>> { D<T> c; };			// { dg-bogus "'class A<D<T>, D<T> >' cannot be specialized" }
#if __cpp_variable_templates >= 201304
template <>
constexpr int b <int, int> = 2;				// { dg-bogus "'b<int, int>' cannot be specialized" }
template <typename T>
constexpr int b <long, T> = 3;				// { dg-bogus "'b<long int, T>' cannot be specialized" }
template <typename T>
constexpr int b <D <T>, D <T>> = 4;			// { dg-bogus "'b<D<T>, D<T> >' cannot be specialized" }
#endif
template <>
int foo <int, int> () { return 1; }			// { dg-bogus "'int foo\\\(\\\) \\\[with T = int; U = int\\\]' cannot be specialized" }
template <typename T, typename U>
struct [[clang::no_specializations ("this is why")]] B {}; // { dg-bogus "declared 'clang::no_specializations' here" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[clang::no_specializations ("my other reason")]] int c = 1; // { dg-bogus "declared 'clang::no_specializations' here" }
#endif
template <typename T, typename U>
[[_Clang::__no_specializations__ ("bar cannot be specialized")]] int bar () { return 0; } // { dg-bogus "declared 'clang::no_specializations' here" }
template <>
struct B <int, int> { int a; };				// { dg-bogus "'struct B<int, int>' cannot be specialized: 'this is why'" }
template <typename T>
struct B <long, T> { long c; };				// { dg-bogus "'struct B<long int, T>' cannot be specialized: 'this is why'" }
template <typename T>
struct B <D <T>, D <T>> { D<T> c; };			// { dg-bogus "'struct B<D<T>, D<T> >' cannot be specialized: 'this is why'" }
#if __cpp_variable_templates >= 201304
template <>
int c <int, int> = 2;					// { dg-bogus "'c<int, int>' cannot be specialized: 'my other reason'" }
template <typename T>
int c <long, T> = 3;					// { dg-bogus "'c<long int, T>' cannot be specialized: 'my other reason'" }
template <typename T>
int c <D <T>, D <T>> = 4;				// { dg-bogus "'c<D<T>, D<T> >' cannot be specialized: 'my other reason'" }
#endif
template <>
int bar <int, int> () { return 1; }			// { dg-bogus "'int bar\\\(\\\) \\\[with T = int; U = int\\\]' cannot be specialized: 'bar cannot be specialized'" }
