// { dg-do compile { target c++20 } }

#include <new>

struct T {
  ~T () noexcept (false);
  static void operator delete (T *, std::destroying_delete_t);
};
T *p = nullptr;
static_assert (noexcept (delete (p)));
