! { dg-do compile }
! { dg-additional-options "-fdump-tree-original" }

! This testcase is ambigous in Fortran
! cf. OpenMP Spec Issues 4998
! Unresolved when this testcase was written

! FIXME: Check what's the outcome of the discussion

! NOTE: GCC currently implements this as spatial dimension
! and not as lower bound ...

subroutine sub
  integer, parameter :: dims(2) = [11,22]
!...
  !$omp teams  num_teams( dims(1) : 256 )  ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
  !$omp end teams
end

! ... hence: 'num_teams(256)' and not 'num_teams(11:256)'

! { dg-final { scan-tree-dump "#pragma omp teams num_teams\\(256\\)" "original" } }
