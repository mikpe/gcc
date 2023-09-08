# this test requires run time library string support which pdp10 doesn't
# support yet
# this test compiles, but doesn't link because it requires strcpy(), strcmp()
set torture_execute_xfail "*-*-*"
return 0
