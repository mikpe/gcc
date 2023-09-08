# this test requires run time library support which pdp10 doesn't
# support yet
# this test compiles, but doesn't link becaue it requires memcmp(), memcpy()
set torture_execute_xfail "*-*-*"
return 0
