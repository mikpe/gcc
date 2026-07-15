// PR c++/120635
// { dg-do compile { target c++17 } }

template <typename T, typename U>
struct [[using _Clang:__no_specializations__]] A {};
template <typename T, typename U>
struct [[using _Clang:__no_specializations__ ("foo")]] B {};
template <typename T, typename U>
struct [[__clang__::__no_specializations__]] C {};		// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" }
template <typename T, typename U>
struct [[__clang__::__no_specializations__ ("foo")]] D {};	// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" }
template <typename T, typename U>
struct [[using __clang__:__no_specializations__]] E {};		// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" }
template <typename T, typename U>
struct [[using __clang__:__no_specializations__ ("foo")]] F {};	// { dg-warning "alternate attribute namespace for 'clang' is '_Clang' rather than '__clang__'" }
