// PR c++/124347
// { dg-do compile { target c++20 } }

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

struct S { char a; int b; char c[32]; char d; };

constexpr bool
plugh ()
{
  S *a = new S {};
  if (foo (a) != sizeof (S)
      || bar (a) != sizeof (S)
      || baz (a) != sizeof (S)
      || qux (a) != sizeof (S)
      || fred (a) != sizeof (S)
      || corge (a) != sizeof (S)
      || garply (a) != sizeof (S)
      || xyzzy (a) != sizeof (S))
    return false;
  char *b = &a->c[2];
  if (foo (b) != sizeof (S) - (&a->c[2]- &a->a)
      || bar (b) != 32 - 2 
      || baz (b) != sizeof (S) - (&a->c[2]- &a->a)
      || qux (b) != 32 - 2 
      || fred (b) != sizeof (S) - (&a->c[2]- &a->a)
      || corge (b) != 32 - 2 
      || garply (b) != sizeof (S) - (&a->c[2]- &a->a)
      || xyzzy (b) != 32 - 2)
    return false;
  delete a;
  return true;
}

static_assert (plugh ());
