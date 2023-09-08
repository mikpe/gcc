# this test requires run time library string support which pdp10 doesn't
# support yet
# marking this as an expected compile failure leaves the run-time test
# as unresolved, but there doesn't seem to be any way of marking that
# the execution test is expected to be unrunable
set torture_compile_xfail "*-*-*"
return 0
