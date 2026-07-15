// { dg-do compile }

// Check that the all non-predefined allocators used in
// target's allocator clauses are in 'uses_allocators'

//#include <omp.h>

typedef __UINTPTR_TYPE__ omp_uintptr_t;

typedef enum omp_alloctrait_key_t
{
  omp_atk_alignment = 2,
} omp_alloctrait_key_t;

typedef enum omp_alloctrait_value_t
{
  omp_atv_default = (__UINTPTR_TYPE__) -1,
} omp_alloctrait_value_t;

typedef struct omp_alloctrait_t
{
  omp_alloctrait_key_t key;
  omp_uintptr_t value;
} omp_alloctrait_t;

typedef enum omp_allocator_handle_t
{
  omp_null_allocator = 0,
  __omp_allocator_handle_t_max__ = __UINTPTR_MAX__
} omp_allocator_handle_t;

typedef enum omp_memspace_handle_t
{
  omp_default_mem_space = 0,
  omp_low_lat_mem_space = 4,
  __omp_memspace_handle_t_max__ = __UINTPTR_MAX__
} omp_memspace_handle_t;


int main ()
{
  const omp_alloctrait_t traits[] = {{omp_atk_alignment, 1024}};
  omp_allocator_handle_t alloc = omp_null_allocator;
  omp_allocator_handle_t alloc2 = omp_null_allocator;

  int array[2], var2 = 124;
  int res = 1;

  #pragma omp target map(from: res) \
     uses_allocators(traits(traits) : alloc2) \
     allocate(alloc : array, var2) private(array) firstprivate(var2)
  {
// { dg-error "allocator 'alloc' requires 'uses_allocators\\(alloc\\)' clause in target region" "" { target *-*-* } .-2 }
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
