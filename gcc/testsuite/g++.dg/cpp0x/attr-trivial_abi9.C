// PR c++/125064
// { dg-do compile { target c++17 } }

struct A;
class [[clang::trivial_abi]] B {
  B(B &&);
};
void f(A);
struct [[clang::trivial_abi]] A { // { dg-warning "cannot be applied" }
  A(...);
  B b;
};
void g() { f(1); }
