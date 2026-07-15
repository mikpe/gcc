// PR c++/120635
// { dg-do compile { target c++11 } }
// { dg-options "-Winvalid-specialization" }

template <typename T, typename U>
class [[clang::no_specializations]] A {};		// { dg-message "declared 'clang::no_specializations' here" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[clang::no_specializations]] constexpr int b = 1;	// { dg-message "declared 'clang::no_specializations' here" "" { target c++14 } }
#endif
template <typename T, typename U>
[[_Clang::__no_specializations__]] int foo () { return 0; }	// { dg-message "declared 'clang::no_specializations' here" }
template <typename T>
struct D {};
template <>
struct A <int, int> { int a; };				// { dg-error "'class A<int, int>' cannot be specialized" }
template <typename T>
struct A <long, T> { long b; };				// { dg-error "'class A<long int, T>' cannot be specialized" }
template <typename T>
struct A <D <T>, D <T>> { D<T> c; };			// { dg-error "'class A<D<T>, D<T> >' cannot be specialized" }
#if __cpp_variable_templates >= 201304
template <>
constexpr int b <int, int> = 2;				// { dg-error "'b<int, int>' cannot be specialized" "" { target c++14 } }
template <typename T>
constexpr int b <long, T> = 3;				// { dg-error "'b<long int, T>' cannot be specialized" "" { target c++14 } }
template <typename T>
constexpr int b <D <T>, D <T>> = 4;			// { dg-error "'b<D<T>, D<T> >' cannot be specialized" "" { target c++14 } }
#endif
template <>
int foo <int, int> () { return 1; }			// { dg-error "'int foo\\\(\\\) \\\[with T = int; U = int\\\]' cannot be specialized" }
template <typename T, typename U>
struct [[clang::no_specializations ("this is why")]] B {}; // { dg-message "declared 'clang::no_specializations' here" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[clang::no_specializations ("my other reason")]] int c = 1; // { dg-message "declared 'clang::no_specializations' here" "" { target c++14 } }
#endif
template <typename T, typename U>
[[_Clang::__no_specializations__ ("bar cannot be specialized")]] int bar () { return 0; } // { dg-message "declared 'clang::no_specializations' here" }
template <>
struct B <int, int> { int a; };				// { dg-error "'struct B<int, int>' cannot be specialized: 'this is why'" }
template <typename T>
struct B <long, T> { long c; };				// { dg-error "'struct B<long int, T>' cannot be specialized: 'this is why'" }
template <typename T>
struct B <D <T>, D <T>> { D<T> c; };			// { dg-error "'struct B<D<T>, D<T> >' cannot be specialized: 'this is why'" }
#if __cpp_variable_templates >= 201304
template <>
int c <int, int> = 2;					// { dg-error "'c<int, int>' cannot be specialized: 'my other reason'" "" { target c++14 } }
template <typename T>
int c <long, T> = 3;					// { dg-error "'c<long int, T>' cannot be specialized: 'my other reason'" "" { target c++14 } }
template <typename T>
int c <D <T>, D <T>> = 4;				// { dg-error "'c<D<T>, D<T> >' cannot be specialized: 'my other reason'" "" { target c++14 } }
#endif
template <>
int bar <int, int> () { return 1; }			// { dg-error "'int bar\\\(\\\) \\\[with T = int; U = int\\\]' cannot be specialized: 'bar cannot be specialized'" }
template <typename T>
struct D2 {};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-specialization"
template <>
struct A <short, int> { int a; };			// { dg-bogus "'class A<int, int>' cannot be specialized" }
template <typename T>
struct A <long long, T> { long b; };			// { dg-bogus "'class A<long int, T>' cannot be specialized" }
template <typename T>
struct A <D2 <T>, D2 <T>> { D2<T> c; };			// { dg-bogus "'class A<D<T>, D<T> >' cannot be specialized" }
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
