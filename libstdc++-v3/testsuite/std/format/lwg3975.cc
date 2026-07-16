// { dg-do compile { target c++20 } }

// LWG 3975. Specializations of basic_format_context should not be permitted

#include <format>

struct A {};
template<typename T>
struct B {};

template<>
class std::basic_format_parse_context<A> {};	// { dg-error "cannot be specialized" }

template<>
class std::basic_format_context<std::back_insert_iterator<std::string>, A> {};	// { dg-error "cannot be specialized" }

template<typename T>
class std::basic_format_context<B<T>, T> {};	// { dg-error "cannot be specialized" }
