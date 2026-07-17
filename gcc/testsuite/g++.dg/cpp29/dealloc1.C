// P3424R2 - Deallocation Functions with Throwing Exception Specification Are Ill-formed
// { dg-do compile { target c++11 } }

struct A {};
struct B {};
void operator delete (void *, A) noexcept (false);	// { dg-error "deallocation function 'void operator delete\\\(void\\\*, A\\\)' declared possibly throwing" "" { target c++29 } }
void operator delete (void *, B) noexcept (false) {}	// { dg-error "deallocation function 'void operator delete\\\(void\\\*, B\\\)' declared possibly throwing" "" { target c++29 } }
void operator delete[] (void *, A) noexcept (false);	// { dg-error "deallocation function 'void operator delete \\\[\\\]\\\(void\\\*, A\\\)' declared possibly throwing" "" { target c++29 } }
void operator delete[] (void *, B) noexcept (false) {}	// { dg-error "deallocation function 'void operator delete \\\[\\\]\\\(void\\\*, B\\\)' declared possibly throwing" "" { target c++29 } }
template <bool T, bool U>
struct C {
  static void operator delete (void *) noexcept (T);	// { dg-error "deallocation function 'static void C<T, U>::operator delete\\\(void\\\*\\\) \\\[with bool T = false; bool U = true\\\]' declared possibly throwing" "" { target c++29 } }
  static void operator delete[] (void *) noexcept (T);	// { dg-error "deallocation function 'static void C<T, U>::operator delete \\\[\\\]\\\(void\\\*\\\) \\\[with bool T = false; bool U = true\\\]' declared possibly throwing" "" { target c++29 } }
};
template <bool T, bool U>
struct D {
  static void operator delete (void *) noexcept (T) {}	// { dg-error "deallocation function 'static void D<T, U>::operator delete\\\(void\\\*\\\) \\\[with bool T = false; bool U = true\\\]' declared possibly throwing" "" { target c++29 } }
  static void operator delete[] (void *) noexcept (T) {}// { dg-error "deallocation function 'static void D<T, U>::operator delete \\\[\\\]\\\(void\\\*\\\) \\\[with bool T = false; bool U = true\\\]' declared possibly throwing" "" { target c++29 } }
};
C <false, false> a;
C <true, false> b;
D <false, false> c;
D <true, false> d;
auto e = &C <false, true>::operator delete;
auto f = &C <true, true>::operator delete;
auto g = &D <false, true>::operator delete;
auto h = &D <true, true>::operator delete;
auto i = &C <false, true>::operator delete[];
auto j = &C <true, true>::operator delete[];
auto k = &D <false, true>::operator delete[];
auto l = &D <true, true>::operator delete[];
struct E {
  static void operator delete (void *) noexcept (C) {}	// { dg-error "deallocation function 'static void E::operator delete\\\(void\\\*\\\)' declared possibly throwing" "" { target c++29 } }
  static constexpr bool C = false;
};
template <int N>
struct F {
  static void operator delete (void *) noexcept (false);	// { dg-error "deallocation function 'static void F<N>::operator delete\\\(void\\\*\\\)' declared possibly throwing" "" { target c++29 } }
  static void operator delete[] (void *) noexcept (false);	// { dg-error "deallocation function 'static void F<N>::operator delete \\\[\\\]\\\(void\\\*\\\)' declared possibly throwing" "" { target c++29 } }
};
template <int N>
struct G {
  static void operator delete (void *) noexcept (false) {}	// { dg-error "deallocation function 'static void G<N>::operator delete\\\(void\\\*\\\)' declared possibly throwing" "" { target c++29 } }
  static void operator delete[] (void *) noexcept (false) {}	// { dg-error "deallocation function 'static void G<N>::operator delete \\\[\\\]\\\(void\\\*\\\)' declared possibly throwing" "" { target c++29 } }
};
