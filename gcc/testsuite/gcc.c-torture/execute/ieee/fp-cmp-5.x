# this test requires compile time floating point expression evaluation.
# its not clear if that's really required, but for now since we don't
# support ieee numbers at all, 
# marking this as an expected compile failure
set torture_compile_xfail "*-*-*"
return 0
