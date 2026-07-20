! { dg-do compile }
! { dg-additional-options "-fdump-tree-original" }

subroutine one_a
!$omp teams num_teams(dims(3) : 1,2,3) &            ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
!$omp&      thread_limit ( dims ( 4 ) : 1, 2, 3, 4) ! { dg-message "sorry, unimplemented: 'thread_limit' with 'dims' modifier" }
!$omp   parallel num_threads(dims(3): 1,2,3)        ! { dg-message "sorry, unimplemented: 'num_threads' with 'dims' modifier" }
!$omp   end parallel
!$omp end teams
end

subroutine one_b
!$omp teams num_teams(dims(3) : 1,2,3) &                      ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
!$omp&      thread_limit ( dims ( 4 ) , relaxed : 1, 2, 3, 4) ! { dg-message "sorry, unimplemented: 'thread_limit' with 'dims' modifier" }
!$omp   parallel num_threads(dims(3) , relaxed : 1,2,3)       ! { dg-message "sorry, unimplemented: 'num_threads' with 'dims' modifier" }
!$omp   end parallel
!$omp end teams
end

subroutine one_c
!$omp teams num_teams(dims(3) : 1,2,3) &                     ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
!$omp&      thread_limit ( dims ( 4 ) , strict : 1, 2, 3, 4) ! { dg-message "sorry, unimplemented: 'thread_limit' with 'dims' modifier" }
!$omp   parallel num_threads(dims(3) , strict : 1,2,3)       ! { dg-message "sorry, unimplemented: 'num_threads' with 'dims' modifier" }
!$omp   end parallel
!$omp end teams
end

subroutine one_d
!$omp teams num_teams ( dims(3) : 1,2,3) &                     ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
!$omp&      thread_limit ( relaxed ,  dims ( 4 ) : 1, 2, 3, 4) ! { dg-message "sorry, unimplemented: 'thread_limit' with 'dims' modifier" }
!$omp   parallel num_threads( relaxed , dims(3) : 1,2,3)       ! { dg-message "sorry, unimplemented: 'num_threads' with 'dims' modifier" }
!$omp   end parallel
!$omp end teams
end

subroutine one_e
!$omp teams num_teams ( dims(3) : 1,2,3) &                    ! { dg-message "sorry, unimplemented: 'num_teams' with 'dims' modifier" }
!$omp&      thread_limit ( strict ,  dims ( 4 ) : 1, 2, 3, 4) ! { dg-message "sorry, unimplemented: 'thread_limit' with 'dims' modifier" }
!$omp   parallel num_threads( strict , dims(3) : 1,2,3)       ! { dg-message "sorry, unimplemented: 'num_threads' with 'dims' modifier" }
!$omp   end parallel
!$omp end teams
end


subroutine two
!$omp teams thread_limit (strict : 4)          ! { dg-message "sorry, unimplemented: 'thread_limit' with 'strict' modifier" }
!$omp   parallel num_threads(strict : 2)       ! { dg-message "sorry, unimplemented: 'num_threads' with 'strict' modifier" }
!$omp   end parallel
!$omp end teams
end

subroutine zero
!$omp teams num_teams(1234) thread_limit ( 2345)  ! OK
!$omp   parallel num_threads( 3456)            ! OK
!$omp   end parallel
!$omp end teams

!$omp teams num_teams(123:456)
!$omp end teams
end

subroutine three
!$omp teams num_teams(4567) thread_limit (relaxed : 5678)  ! OK
!$omp   parallel num_threads(relaxed : 6789)            ! OK
!$omp   end parallel
!$omp end teams
end

subroutine four
!$omp parallel num_threads(1 , 2 , 3 )  ! { dg-message "sorry, unimplemented: 'num_threads' with more than one argument" }
!$omp end parallel
end

subroutine five
!$omp parallel num_threads(relaxed : 1,2,3)  ! { dg-message "sorry, unimplemented: 'num_threads' with more than one argument" }
!$omp end parallel
end

subroutine six
!$omp parallel num_threads(strict : 1,2,3)  ! { dg-message "sorry, unimplemented: 'num_threads' with 'strict' modifier" }
!$omp end parallel
end


subroutine seven
  integer :: static, relaxed, dims(2)
  !$omp teams num_teams(static) thread_limit (static)  ! OK
  !$omp   parallel num_threads(static)                 ! OK
  !$omp   end parallel
  !$omp end teams

  !$omp teams num_teams(relaxed) thread_limit (relaxed)  ! OK
  !$omp   parallel num_threads(relaxed)                  ! OK
  !$omp   end parallel
  !$omp end teams

  !$omp teams num_teams(dims(2)) thread_limit (dims(2))  ! OK
  !$omp   parallel num_threads(dims(2))                  ! OK
  !$omp   end parallel
  !$omp end teams

end subroutine

! Check that the tree is correctly generated:

! { dg-final { scan-tree-dump-times "#pragma omp teams num_teams\\(1234\\) thread_limit\\(2345\\)" 1 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp parallel num_threads\\(3456\\)" 1 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp teams num_teams\\(4567\\) thread_limit\\(5678\\)" 1 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp parallel num_threads\\(6789\\)" 1 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp teams num_teams\\(123:456\\)" 1 "original" } }

! { dg-final { scan-tree-dump-times "D\\.\[0-9\]+ = static;" 3 "original" } }
! { dg-final { scan-tree-dump-times "D\\.\[0-9\]+ = relaxed;" 3 "original" } }
! { dg-final { scan-tree-dump-times "D\\.\[0-9\]+ = dims\\\[1\\\];" 3 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp teams num_teams\\(D\\.\[0-9\]+\\) thread_limit\\(D\\.\[0-9\]+\\)" 3 "original" } }
! { dg-final { scan-tree-dump-times "#pragma omp parallel num_threads\\(D\\.\[0-9\]+\\)" 3 "original" } }
