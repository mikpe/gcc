// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test std::meta::is_noexcept.

#include <meta>
#include <new>

struct A {
  ~A ();
};
static_assert (is_noexcept (^^A::~A));
struct B {
  ~B () noexcept (false);
};
static_assert (!is_noexcept (^^B::~B));
struct C {
  ~C () noexcept (false);
  static void operator delete (void *);
};
static_assert (is_noexcept (^^C::operator delete));
struct D {
  ~D () noexcept (false);
  static void operator delete (D *, std::destroying_delete_t);
};
static_assert (is_noexcept (^^D::operator delete));
