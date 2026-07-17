// { dg-do compile { target c++11 } }

struct A {};
void operator delete (void *, A);
void operator delete (void *, A) noexcept (false);	// { dg-error "declaration of 'void operator delete\\\(void\\\*, A\\\) noexcept \\\(false\\\)' has a different exception specifier" }
// { dg-error "deallocation function 'void operator delete\\\(void\\\*, A\\\)' declared possibly throwing" "" { target c++29 } .-1 }
struct B {};
void operator delete (void *, B) noexcept (false);	// { dg-error "deallocation function 'void operator delete\\\(void\\\*, B\\\)' declared possibly throwing" "" { target c++29 } }
void operator delete (void *, B);			// { dg-error "declaration of 'void operator delete\\\(void\\\*, B\\\) noexcept' has a different exception specifier" }
