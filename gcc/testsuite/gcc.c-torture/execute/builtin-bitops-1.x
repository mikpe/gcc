# this test requires run time library support which pdp10 doesn't
# support yet
# this test compiles, but doesn't link becaue it requires
# POPCOUNTSI2(), PARITYSI2(), PARITYDI2(), CLZSI2(), CTZDI2(),
# CLZDI2(), FFSDI2(), POPCOUNTDI2(), CTZSI2()
set torture_execute_xfail "*-*-*"
return 0
