# this test requires run time library string support which pdp10 doesn't
# support yet
# marking this as an expected compile failure leaves the run-time test
# as unresolved, but there doesn't seem to be any way of marking that
# the execution test is expected to be unrunable
set torture_compile_xfail "*-*-*"
# VxWorks kernel mode has the same problem.
if {[istarget "*-*-vxworks*"]} {
    set torture_eval_before_execute {
	global compiler_conditional_xfail_data
	set compiler_conditional_xfail_data {
	    "The kernel HUGE_VAL is defined to DBL_MAX instead of +Inf."
	    { "*-*-*" }
	    {}
	    { "-mrtp" }
	}
    }
}

return 0
