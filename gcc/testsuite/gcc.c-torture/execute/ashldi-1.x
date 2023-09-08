# This test assumes long longs are either 32 or 64 bits and needs to be rewritten
# for pdp10 71 or 72 bit long longs.  Currently, because it assumes long longs
# are one of those sizes, it fails to compile.
# marking this as an expected compile failure leaves the run-time test
# as unresolved, but there doesn't seem to be any way of marking that
# the execution test is expected to be unrunable
set torture_compile_xfail "*-*-*"
return 0
