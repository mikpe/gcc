// PR c++/120635
// { dg-do compile { target c++11 } }

int arr[2];
struct S { int a, b; };
S arr2[2];

template <typename T>
struct [[clang::no_specializations]] S1 {};
template <typename T>
struct [[clang::no_specializations ("foobar")]] S2 {};
template <typename T>
struct [[clang::no_specializations (0)]] S3 {};			// { dg-error "'no_specializations' attribute argument must be a string constant|expected string-literal" }
template <typename T>
struct [[clang::no_specializations ("foo", "bar", "baz")]] S4 {};// { dg-error "wrong number of arguments specified for 'no_specializations' attribute" }
template <typename T>
struct [[clang::no_specializations (0, 1, 2)]] S5 {};		// { dg-error "wrong number of arguments specified for 'no_specializations' attribute|expected string-literal" }

void
foo (int n)
{
  auto a = [] [[clang::no_specializations]] () {};		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  auto b = [] constexpr [[clang::no_specializations]] {};	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
								// { dg-error "parameter declaration before lambda declaration specifiers only optional with" "" { target c++20_down } .-1 }
								// { dg-error "'constexpr' lambda only available with" "" { target c++14_down } .-2 }
  auto c = [] noexcept [[clang::no_specializations]] {};	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
								// { dg-error "parameter declaration before lambda exception specification only optional with" "" { target c++20_down } .-1 }
  auto d = [] () [[clang::no_specializations]] {};		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  auto e = new int [n] [[clang::no_specializations]];		// { dg-warning "attributes ignored on outermost array type in new expression" }
  auto e2 = new int [n] [[clang::no_specializations]] [42];	// { dg-warning "attributes ignored on outermost array type in new expression" }
  auto f = new int [n][42] [[clang::no_specializations]];	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]];				// { dg-warning "attributes at the beginning of statement are ignored" }
  [[clang::no_specializations]] {}				// { dg-warning "attributes at the beginning of statement are ignored" }
  [[clang::no_specializations]] if (true) {}			// { dg-warning "attributes at the beginning of statement are ignored" }
  [[clang::no_specializations]] while (false) {}		// { dg-warning "attributes at the beginning of statement are ignored" }
  [[clang::no_specializations]] goto lab;			// { dg-warning "attributes at the beginning of statement are ignored" }
  [[clang::no_specializations]] lab:;				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] try {} catch (int) {}		// { dg-warning "attributes at the beginning of statement are ignored" }
  if ([[clang::no_specializations]] int x = 0) {}		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  switch (n)
    {
    [[clang::no_specializations]] case 1:			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
    [[clang::no_specializations]] break;			// { dg-warning "attributes at the beginning of statement are ignored" }
    [[clang::no_specializations]] default:			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
	 break;
    }
  for ([[clang::no_specializations]] auto a : arr) {}		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  for ([[clang::no_specializations]] auto [a, b] : arr2) {}	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
								// { dg-error "structured bindings only available with" "" { target c++14_down } .-1 }
  [[clang::no_specializations]] asm ("");			// { dg-warning "attributes ignored on 'asm' declaration" }
  try {} catch ([[clang::no_specializations]] int x) {}		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  try {} catch ([[clang::no_specializations]] int) {}		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  try {} catch (int [[clang::no_specializations]] x) {}		// { dg-warning "attribute ignored" }
  try {} catch (int [[clang::no_specializations]]) {}		// { dg-warning "attribute ignored" }
  try {} catch (int x [[clang::no_specializations]]) {}		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
}

[[clang::no_specializations]] int bar ();			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
using foobar [[clang::no_specializations]] = int;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
[[clang::no_specializations]] int a;				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
[[clang::no_specializations]] auto [b, c] = arr;		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
								// { dg-error "structured bindings only available with" "" { target c++14_down } .-1 }
[[clang::no_specializations]];					// { dg-warning "attribute ignored" }
inline [[clang::no_specializations]] void baz () {}		// { dg-warning "attribute ignored" }
								// { dg-error "standard attributes in middle of decl-specifiers" "" { target *-*-* } .-1 }
constexpr [[clang::no_specializations]] int qux () { return 0; }// { dg-warning "attribute ignored" }
								// { dg-error "standard attributes in middle of decl-specifiers" "" { target *-*-* } .-1 }
int [[clang::no_specializations]] d;				// { dg-warning "attribute ignored" }
int const [[clang::no_specializations]] e = 1;			// { dg-warning "attribute ignored" }
struct A {} [[clang::no_specializations]];			// { dg-warning "attribute ignored in declaration of 'struct A'" }
struct A [[clang::no_specializations]];				// { dg-warning "attribute ignored" }
struct A [[clang::no_specializations]] a1;			// { dg-warning "attribute ignored" }
A [[clang::no_specializations]] a2;				// { dg-warning "attribute ignored" }
enum B { B0 } [[clang::no_specializations]];			// { dg-warning "attribute ignored in declaration of 'enum B'" }
enum B [[clang::no_specializations]];				// { dg-warning "attribute ignored" }
enum B [[clang::no_specializations]] b1;			// { dg-warning "attribute ignored" }
B [[clang::no_specializations]] b2;				// { dg-warning "attribute ignored" }
struct [[clang::no_specializations]] C {};			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int f [[clang::no_specializations]];				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int g[2] [[clang::no_specializations]];				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int g2 [[clang::no_specializations]] [2];			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int corge () [[clang::no_specializations]];			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int *[[clang::no_specializations]] h;				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int & [[clang::no_specializations]] i = f;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int && [[clang::no_specializations]] j = 0;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int S::* [[clang::no_specializations]] k;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
auto l = sizeof (int [2] [[clang::no_specializations]]);	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
int freddy ([[clang::no_specializations]] int a,		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
	    [[clang::no_specializations]] int,			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
	    [[clang::no_specializations]] int c = 0,		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
	    [[clang::no_specializations]] int = 0);		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
void
corge ([[clang::no_specializations]] int a,			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
       [[clang::no_specializations]] int,			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
       [[clang::no_specializations]] int c = 0,			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
       [[clang::no_specializations]] int = 0)			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
{
}
[[clang::no_specializations]] void
garply ()							// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
{
}
[[clang::no_specializations]] int
xyzzyy ()							// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
{
  return 0;
}
int grault (int [[clang::no_specializations]] a,		// { dg-warning "attribute ignored" }
	    int [[clang::no_specializations]],			// { dg-warning "attribute ignored" }
	    int [[clang::no_specializations]] c = 0,		// { dg-warning "attribute ignored" }
	    int [[clang::no_specializations]] = 0);		// { dg-warning "attribute ignored" }
void
waldo (int [[clang::no_specializations]] a,			// { dg-warning "attribute ignored" }
       int [[clang::no_specializations]],			// { dg-warning "attribute ignored" }
       int [[clang::no_specializations]] c = 0,			// { dg-warning "attribute ignored" }
       int [[clang::no_specializations]] = 0)			// { dg-warning "attribute ignored" }
{
}
int plugh (int a [[clang::no_specializations]],			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
	    int b [[clang::no_specializations]] = 0);		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
void
thud (int a [[clang::no_specializations]],			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
      int b [[clang::no_specializations]] = 0)			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
{
}
enum [[clang::no_specializations]] D { D0 };			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
enum class [[clang::no_specializations]] E { E0 };		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
enum F {};
enum [[clang::no_specializations]] F;				// { dg-warning "type attributes ignored after type is already defined" }
enum G {
  G0 [[clang::no_specializations]],				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  G1 [[clang::no_specializations]] = 2				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
};
namespace [[clang::no_specializations]] H { using H0 = int; }	// { dg-warning "'no_specializations' attribute directive ignored" }
namespace [[clang::no_specializations]] {}			// { dg-warning "'no_specializations' attribute directive ignored" }
[[clang::no_specializations]] using namespace H;		// { dg-warning "'no_specializations' attribute directive ignored" }
struct [[clang::no_specializations]] I				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
{
  [[clang::no_specializations]];				// { dg-error "declaration does not declare anything" }
  [[clang::no_specializations]] int i;				// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] int foo ();			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] int bar () { return 1; }	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] int : 0;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] int i2 : 5;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  [[clang::no_specializations]] static int i3;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
  static int i4;
};
[[clang::no_specializations]] int I::i4 = 0;			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
struct J : [[clang::no_specializations]] C {};			// { dg-warning "attributes on base specifiers are ignored" }
#if __cpp_concepts >= 201907L
template <typename T>
concept K [[clang::no_specializations]] = requires { true; };	// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" "" { target c++20 } }
#endif
typedef int L [[clang::no_specializations]];			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
template <typename T>
struct M {};
template <>
struct [[clang::no_specializations]] M<int> { int m; };		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
typedef int N[2] [[clang::no_specializations]];			// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
typedef int O [[clang::no_specializations]] [2];		// { dg-warning "'no_specializations' attribute only applies to class, function or variable templates" }
