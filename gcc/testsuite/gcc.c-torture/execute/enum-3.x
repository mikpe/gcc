# this test requires run time library string support which pdp10 doesn't
# support yet
# this test includes stdio.h which results in a compile error
# the execution test is therefore unresolvable
set torture_compile_xfail "*-*-*"
return 0
