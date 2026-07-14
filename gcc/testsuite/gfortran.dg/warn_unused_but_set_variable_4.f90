! { dg-do compile }
! { dg-additional-options "-Wall" }
! PR 126058 - this used to give an ICE.
PROGRAM p
  IMPLICIT NONE
  TYPE t1
     INTEGER, ALLOCATABLE :: i(:)
  END TYPE t1
  call leak
CONTAINS
  SUBROUTINE s1(e)
    TYPE(t1), ALLOCATABLE, INTENT(OUT) :: e(:)
    ALLOCATE( e(1) )
    ALLOCATE( e(1)%i(2) )
  END SUBROUTINE s1
  SUBROUTINE leak
    TYPE(t1), ALLOCATABLE :: e(:)
    CALL s1(e) ! { dg-warning "maybe allocated" }
    CALL s1(e)
  END SUBROUTINE leak
END PROGRAM p
