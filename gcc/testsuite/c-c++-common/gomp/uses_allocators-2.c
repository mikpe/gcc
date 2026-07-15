// { dg-do compile }

// PR c/122748
// Was previously ICEing

void f()
{
  int x;
#pragma omp target parallel \
    uses_allocators(omp_thread_mem_alloc) \
    allocate(omp_thread_mem_alloc: x) private(x)
  { }
}

// C
// { dg-error "'omp_thread_mem_alloc' undeclared \\(first use in this function\\)" "" { target c } 10 }
// { dg-note  "each undeclared identifier is reported only once for each function it appears in" "" { target c } 10 }
// { dg-error "allocate' clause must specify an allocator here" "" { target c } 11 }

// C++
// { dg-error "'omp_thread_mem_alloc' has not been declared" "" { target c++ } 10 }
// { dg-error "'omp_thread_mem_alloc' was not declared in this scope" "" { target c++ } 11 }
// { dg-error "'allocate' clause must specify an allocator here" "" { target c++ } 11 }
