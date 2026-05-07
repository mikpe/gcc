// PR c++/124347
// { dg-do compile { target c++11 } }

using size_t = decltype (sizeof 0);

constexpr size_t
foo (void *p)
{
  return __builtin_object_size (p, 0);
}

constexpr size_t
bar (void *p)
{
  return __builtin_object_size (p, 1);
}

constexpr size_t
baz (void *p)
{
  return __builtin_object_size (p, 2);
}

constexpr size_t
qux (void *p)
{
  return __builtin_object_size (p, 3);
}

constexpr size_t
fred (void *p)
{
  return __builtin_dynamic_object_size (p, 0);
}

constexpr size_t
corge (void *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

constexpr size_t
garply (void *p)
{
  return __builtin_dynamic_object_size (p, 2);
}

constexpr size_t
xyzzy (void *p)
{
  return __builtin_dynamic_object_size (p, 3);
}

struct S { char a; int b; char c[32]; } a[10];
static_assert (foo (&a[2].c[5]) == &a[9].a - &a[2].c[5] + sizeof (S), "");
static_assert (bar (&a[2].c[5]) == 32 - 5, "");
static_assert (baz (&a[2].c[5]) == &a[9].a - &a[2].c[5] + sizeof (S), "");
static_assert (qux (&a[2].c[5]) == 32 - 5, "");
static_assert (fred (&a[2].c[5]) == &a[9].a - &a[2].c[5] + sizeof (S), "");
static_assert (corge (&a[2].c[5]) == 32 - 5, "");
static_assert (garply (&a[2].c[5]) == &a[9].a - &a[2].c[5] + sizeof (S), "");
static_assert (xyzzy (&a[2].c[5]) == 32 - 5, "");
