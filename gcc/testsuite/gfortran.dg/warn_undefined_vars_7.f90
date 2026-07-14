! { dg-do compile }
! { dg-additional-options "-Wundefined-vars" }

! Allocatable variables passed to INTENT(OUT) formal arguments can
! only be considered allocated if the formal argument is allocatable.

module m
  implicit none
  interface
     subroutine no_allocation(x)
       real, dimension(:), intent(out) :: x
     end subroutine no_allocation
  end interface
  interface
     subroutine allocation(x)
       real, dimension(:), intent(out), allocatable :: x
     end subroutine allocation
  end interface
contains
  subroutine foo
    real, allocatable :: x(:)
    call no_allocation(x)
    print *,x ! { dg-warning "Unallocated variable.*referenced" }
  end subroutine foo
  subroutine bar
    real, allocatable :: x(:)
    call allocation(x)
    print *,x
  end subroutine bar
end module m
