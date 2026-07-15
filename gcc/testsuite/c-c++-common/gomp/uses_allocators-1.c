// { dg-do compile }
// { dg-additional-options "-Wall -Wno-deprecated-openmp" }

// Check check that there is no 'unsed but set' warning for 'traits'

// #include <omp.h>

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


extern void *omp_alloc (__SIZE_TYPE__, omp_allocator_handle_t);
extern void omp_free (void *, omp_allocator_handle_t);

// ------------------------

int main() {
  double x;  // { dg-warning "variable 'x' set but not used" }

  const omp_alloctrait_t traits1[] = {{omp_atk_alignment, 1024}};  // { dg-bogus "variable 'traits1' set but not used" }
  const omp_alloctrait_t traits2[] = {{omp_atk_alignment, 1024}};  // { dg-bogus "variable 'traits2' set but not used" }
  const omp_alloctrait_t traits3[] = {{omp_atk_alignment, 1024}};  // { dg-bogus "variable 'traits3' set but not used" }
  omp_allocator_handle_t alloc1 = omp_null_allocator;
  omp_allocator_handle_t alloc2 = omp_null_allocator;
  omp_allocator_handle_t alloc3 = omp_null_allocator;

  #pragma omp target \
     uses_allocators(traits(traits1), memspace(omp_low_lat_mem_space) : alloc1) \
     uses_allocators(memspace(omp_default_mem_space), traits(traits2) : alloc2) \
     uses_allocators(alloc3(traits3))
    {
      __builtin_printf ("%p\n", (void*) alloc1);
      __builtin_printf ("%p\n", (void*) alloc2);
      __builtin_printf ("%p\n", (void*) alloc3);
      int *ptr1 = (int*) omp_alloc(sizeof(int), alloc1);
      int *ptr2 = (int*) omp_alloc(sizeof(int), alloc2);
      int *ptr3 = (int*) omp_alloc(sizeof(int), alloc3);
      x = 5;
      *ptr1 = 5;
      *ptr2 = 6;
      *ptr3 = 7;
      omp_free (ptr1, omp_null_allocator);
      omp_free (ptr2, omp_null_allocator);
      omp_free (ptr3, omp_null_allocator);
    }
}
