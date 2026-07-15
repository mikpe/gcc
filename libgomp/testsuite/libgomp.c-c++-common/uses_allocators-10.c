// { dg-do run }

#include <omp.h>

int main ()
{
  const omp_alloctrait_t traits[] = {{omp_atk_alignment, 1024}};
  omp_allocator_handle_t alloc = omp_null_allocator;

  int array[2], var2 = 124;
  int res = 1;

  #pragma omp target map(from: res) \
     uses_allocators(memspace(omp_default_mem_space), traits(traits) : alloc) \
     allocate(alloc : array, var2) private(array) firstprivate(var2)
  {
     array[0] = 1;
     array[1] = 2;
     if (var2 != 124 || ((omp_uintptr_t) &var2) % 1024 != 0)
       __builtin_abort ();
     else if (array[0] != 1 || array[1] != 2 || ((omp_uintptr_t) &array[0]) % 1024 != 0)
       __builtin_abort ();
     else
       res = 42;
  }
  if (res != 42)
    __builtin_abort ();
}
