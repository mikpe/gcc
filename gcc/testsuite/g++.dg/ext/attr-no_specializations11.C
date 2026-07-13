// PR c++/120635
// { dg-do compile { target c++11 } }

template <typename T, typename U>
struct [[gnu::no_specializations]] A {};			// { dg-warning "'\\\[\\\[gnu::no_specializations\\\]\\\]' is not supported; use '\\\[\\\[clang::no_specializations\\\]\\\]' or '__attribute__\\\(\\\(no_specializations\\\)\\\)' instead" }
#if __cpp_variable_templates >= 201304
template <typename T, typename U>
[[gnu::no_specializations ("foo")]] int b = 1;			// { dg-warning "'\\\[\\\[gnu::no_specializations\\\]\\\]' is not supported; use '\\\[\\\[clang::no_specializations\\\]\\\]' or '__attribute__\\\(\\\(no_specializations\\\)\\\)' instead" "" { target c++14 } }
#endif
template <typename T, typename U>
[[__gnu__::__no_specializations__]] int foo () { return 0; }	// { dg-warning "'\\\[\\\[gnu::no_specializations\\\]\\\]' is not supported; use '\\\[\\\[clang::no_specializations\\\]\\\]' or '__attribute__\\\(\\\(no_specializations\\\)\\\)' instead" }
