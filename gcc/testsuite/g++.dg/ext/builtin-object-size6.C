// PR c++/124347
// { dg-do compile { target c++20 } }

constexpr int
foo (const char *p)
{
  return __builtin_dynamic_object_size (p, 0);
}

constexpr int
bar (int x)
{
  char *a = new char[x];
  int b = foo (a);
  delete [] a;
  return b;
}

constexpr int
baz (const char *p)
{
  return __builtin_object_size (p, 1);
}

constexpr int
qux (int x)
{
  char *a = new char[x];
  int b = baz (a);
  delete [] a;
  return b;
}

static_assert (bar (42) == 42);
static_assert (qux (142) == 142);
constexpr char a[] = "foobar";
static_assert (foo (a) == 7);
static_assert (baz (a) == 7);
static_assert (foo ("foo") == 4);
static_assert (baz ("baz") == 4);
