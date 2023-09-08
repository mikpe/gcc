# this test requires run time library support which pdp10 doesn't
# support yet
# this test compiles, but doesn't link becaue it requires ucmpdi2()
# because of use of long long
set torture_execute_xfail "*-*-*"
return 0
