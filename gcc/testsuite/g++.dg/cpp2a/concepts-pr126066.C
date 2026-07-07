// PR c++/126066
// { dg-do compile { target c++20 } }
// { dg-options "-Wall" }

struct S { S& operator=(int); };

void
fn ()
{
  if constexpr (requires(int &a) { a = 0; }) { }
  if constexpr (requires(S &s) { s = 0; }) { }
}

constexpr bool b = requires(int &a) { a = 0; };
static_assert(requires(int &a) { a = 0; });
