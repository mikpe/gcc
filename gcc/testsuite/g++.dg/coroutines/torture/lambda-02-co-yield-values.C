//  { dg-do run }
//  { dg-skip-if "requires hosted libstdc++ for cstdlib abort" { ! hostedlib } }

// lambda with parm and local state

#include "../coro.h"

// boiler-plate for tests of codegen
#include "../coro1-ret-int-yield-int.h"

int main ()
{
  auto f = [](int start) -> coro1
  {
    int value = start;
    PRINT ("f: about to yield start");
    co_yield start;

    value -= 31;
    PRINT ("f: about to yield (value-31)");
    co_yield value;

    value += 6163;
    PRINT ("f: about to return (value+6163)");
    co_return value;
  };

  PRINT ("main: create coro1");
  coro1 x = f (42);
  PRINT ("main: got coro1 - resuming (1)");
  if (x.handle.done())
    abort();
  x.handle.resume();
  PRINT ("main: after resume (1)");
  int y = x.handle.promise().get_value();
  if ( y != 42 )
    abort ();
  PRINT ("main: apparently got 42 - resuming (2)");
  if (x.handle.done())
    abort();
  x.handle.resume();
  PRINT ("main: after resume (2)");
  y = x.handle.promise().get_value();
  if ( y != 11 )
    abort ();
  PRINT ("main: apparently got 11 - resuming (3)");
  if (x.handle.done())
    {
   PRINT ("main: done?");
   abort();
    }
  x.handle.resume();
  PRINT ("main: after resume (2) checking return");
  y = x.handle.promise().get_value();
  if ( y != 6174 )
    abort ();
  PRINT ("main: apparently got 6174");
  if (!x.handle.done())
    {
      PRINT ("main: apparently not done...");
      abort ();
    }
  PRINT ("main: returning");
  return 0;
}
