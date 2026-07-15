// { dg-do compile { target c++11 } }
// Test basic functionality of [[clang::trivial_abi]] attribute

struct [[_Clang::__trivial_abi__]] A { int a; ~A () {} };
#if __cplusplus >= 201703L
struct [[using _Clang:__trivial_abi__]] B { int b; ~B () {} };
#endif
struct [[__clang__::__trivial_abi__]] C { int c; ~C () {} };		// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" }
#if __cplusplus >= 201703L
struct [[using __clang__:__trivial_abi__]] D { int d; ~D () {} };	// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" "" { target c++17 } }
#endif
