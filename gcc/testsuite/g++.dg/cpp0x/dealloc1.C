// { dg-do compile { target c++11 } }

struct A {};
void operator delete (void *, A);
void operator delete (void *, A) noexcept;
struct B {};
void operator delete (void *, B) noexcept (true);
void operator delete (void *, B);
