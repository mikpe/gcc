! { dg-do compile }
! { dg-additional-options "-fdump-tree-gimple" }

!$omp target
!$omp end target

!$omp target device_type ( any )
!$omp end target

!$omp target device_type ( nohost )
!$omp end target

!$omp target device_type ( host )  ! { dg-message "sorry, unimplemented: the 'device_type\\(host\\)' is not supported" }
!$omp end target

end

! { dg-final { scan-tree-dump-times "#pragma omp target num_teams\\(-2\\) thread_limit\\(0\\)\[\\r\\n\]" 1 "gimple" } }
! { dg-final { scan-tree-dump-times "#pragma omp target num_teams\\(-2\\) thread_limit\\(0\\) device_type\\(any\\)" 1 "gimple" } }
! { dg-final { scan-tree-dump-times "#pragma omp target num_teams\\(-2\\) thread_limit\\(0\\) device_type\\(nohost\\)" 1 "gimple" } }
! { dg-final { scan-tree-dump-times "#pragma omp target num_teams\\(-2\\) thread_limit\\(0\\) device_type\\(host\\)" 1 "gimple" } }
