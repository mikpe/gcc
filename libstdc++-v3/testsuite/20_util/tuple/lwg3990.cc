// { dg-do compile { target c++11 } }

// LWG 3990. Program-defined specializations of std::tuple and std::variant
// can't be properly supported

#include <tuple>

struct A {};
template<typename T>
struct B {};

template<>
class std::tuple<A, A> {};	// { dg-error "cannot be specialized" }

template<typename T>
class std::tuple<B<T>, T> {};	// { dg-error "cannot be specialized" }
