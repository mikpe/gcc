// PR c++/120635
// { dg-do preprocess { target c++11 } }

#if __has_cpp_attribute (clang::no_specializations) != 1
#error
#endif
#if __has_cpp_attribute (gnu::no_specializations) != 0
#error
#endif
#if __has_cpp_attribute (no_specializations) != 0
#error This fails for now
// { dg-bogus "This fails for now" "" { xfail *-*-* } .-1 }
#endif
#if __has_attribute (clang::no_specializations) != 1
#error
#endif
#if __has_attribute (gnu::no_specializations) != 0
#error
#endif
#if __has_attribute (no_specializations) != 1
#error
#endif
#if __has_cpp_attribute (_Clang::__no_specializations__) != 1
#error
#endif
#if __has_cpp_attribute (__gnu__::__no_specializations__) != 0
#error
#endif
#if __has_cpp_attribute (__no_specializations__) != 0
#error This fails for now
// { dg-bogus "This fails for now" "" { xfail *-*-* } .-1 }
#endif
#if __has_attribute (_Clang::__no_specializations__) != 1
#error
#endif
#if __has_attribute (__gnu__::__no_specializations__) != 0
#error
#endif
#if __has_attribute (__no_specializations__) != 1
#error
#endif
#if __has_cpp_attribute (__clang__::__no_specializations__) != 0
#error
#endif
#if __has_attribute (__clang__::__no_specializations__) != 0
#error
#endif
