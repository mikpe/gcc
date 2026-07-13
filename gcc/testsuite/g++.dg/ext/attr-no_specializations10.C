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
