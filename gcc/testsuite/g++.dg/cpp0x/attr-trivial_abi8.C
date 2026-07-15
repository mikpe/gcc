// { dg-do preprocess { target c++11 } }

#if __has_cpp_attribute (clang::trivial_abi) != 1
#error
#endif
#if __has_cpp_attribute (gnu::trivial_abi) != 0
#error
#endif
#if __has_cpp_attribute (trivial_abi) != 0
#error This fails for now
// { dg-bogus "This fails for now" "" { xfail *-*-* } .-1 }
#endif
#if __has_attribute (clang::trivial_abi) != 1
#error
#endif
#if __has_attribute (gnu::trivial_abi) != 0
#error
#endif
#if __has_attribute (trivial_abi) != 1
#error
#endif
#if __has_cpp_attribute (_Clang::__trivial_abi__) != 1
#error
#endif
#if __has_cpp_attribute (__gnu__::__trivial_abi__) != 0
#error
#endif
#if __has_cpp_attribute (__trivial_abi__) != 0
#error This fails for now
// { dg-bogus "This fails for now" "" { xfail *-*-* } .-1 }
#endif
#if __has_attribute (_Clang::__trivial_abi__) != 1
#error
#endif
#if __has_attribute (__gnu__::__trivial_abi__) != 0
#error
#endif
#if __has_attribute (__trivial_abi__) != 1
#error
#endif
#if __has_cpp_attribute (__clang__::__trivial_abi__) != 0
#error
#endif
#if __has_attribute (__clang__::__trivial_abi__) != 0
#error
#endif
