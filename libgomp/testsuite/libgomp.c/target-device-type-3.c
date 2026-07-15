/* { dg-do run } */
/* { dg-additional-options -fdump-tree-ompexp } */
/* { dg-additional-options -foffload-options=-fdump-tree-optimized-raw-asmname } */

/* This code is supposed to run on the host independent of the current
   default-device-var ICV - and it shall not create any device code for
   neither the target region nor the called functions.  */

// NOTE: With offloading,
//   UNRESOLVED:
// is expected for the "optimized" tests as those dumps are only enabled
// for the no-host side.


// Due to 'device_type(nohost)', the outlined region shall not be marked
// as 'target entrypoint' - nor should 'ggg' become implicitly
// 'declare target' -- check for the absence of those __attributes__:

/* { dg-final { scan-tree-dump-times "omp declare target" 1 "ompexp" } } */
/* { dg-final { scan-tree-dump-not "omp target entrypoint" "ompexp" } } */


// Expect that GOMP_target explicitly invokes the host device, i.e.
// -3 == (shifted) omp_initial_device (aka host fallback)

/* { dg-final { scan-tree-dump "__builtin_GOMP_target_ext \\(-3," "ompexp" } } */


// On the device side, expect only hhh:

/* { dg-final { scan-tree-dump "hhh" "optimized" { target offload_device } } } */
/* { dg-final { scan-tree-dump-not "ggg" "optimized" { target offload_device } } } */
/* { dg-final { scan-tree-dump-not "_omp_fn" "optimized" { target offload_device } } } */


#include <omp.h>

// Use 'declare target' such that -fdump-tree-optimized creates a file.
// Otherwise, there is dump file (only a stub assembly file)
// That's the only purpose of this function.
int hhh() { return 5; }
#pragma omp declare target enter(hhh)


int ggg() {
  return 42;
}

int f(int *x) {
  bool initial_dev = false;
  #pragma omp target map(tofrom: initial_dev) device_type(host) 
    {
      initial_dev = omp_is_initial_device ();
      (*x) = (*x) + 1 + ggg();
    }
  if (!initial_dev)
    __builtin_abort ();
  return *x;
}

int main() {
  int i = 5;
  if (f(&i) != 5 + 1 + 42)
    __builtin_abort();
}
