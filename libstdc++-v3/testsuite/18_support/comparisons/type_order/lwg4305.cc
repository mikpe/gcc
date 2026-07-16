// { dg-do compile { target c++26 } }

// LWG 4305. Missing user requirements on type_order template

#include <compare>

struct A {};
template<typename T>
struct B {};

template<>
struct std::type_order<A, A> {};		// { dg-error "cannot be specialized" }

template<typename T>
struct std::type_order<B<T>, T> {};		// { dg-error "cannot be specialized" }
