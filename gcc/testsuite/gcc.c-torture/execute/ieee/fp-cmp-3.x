# this test requires run time library string support which pdp10 doesn't
# support yet
# marking this as an expected compile failure leaves the run-time test
# as unresolved, but there doesn't seem to be any way of marking that
# the execution test is expected to be unrunable
set torture_compile_xfail "*-*-*"
return 0

# The ARM VxWorks kernel uses an external floating-point library in
# which routines like __ledf2 are just aliases for __cmpdf2.  These
# routines therefore don't handle NaNs correctly.
if [istarget "arm*-*-vxworks*"] {
    set torture_eval_before_execute {
	global compiler_conditional_xfail_data
	set compiler_conditional_xfail_data {
	    "The ARM kernel uses a flawed floating-point library."
	    { "*-*-*" }
	    {}
	    { "-mrtp" }
	}
    }
}

return 0
