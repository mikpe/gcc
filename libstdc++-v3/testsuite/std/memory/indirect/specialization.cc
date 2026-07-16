// { dg-do compile { target c++26 } }

#include <memory>

struct A {};
template<typename T>
struct B {};

template<>
class std::indirect<A, std::pmr::polymorphic_allocator <A>> {};		// { dg-error "cannot be specialized" }

template<typename T>
class std::indirect<B<T>, std::pmr::polymorphic_allocator <B<T>>> {};	// { dg-error "cannot be specialized" }
