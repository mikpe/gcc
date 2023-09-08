# this test requires run time library string support which pdp10 doesn't
# support yet
# marking this as an expected compile failure leaves the run-time test
# as unresolved, but there doesn't seem to be any way of marking that
# the execution test is expected to be unrunable
set torture_compile_xfail "*-*-*"
return 0
# judging by the following gcc customizations, it must be possible to
# do sophisticated test flagging.  It sure would be nice if I could find
# documentation on it.
# mtc 11/30/2007


if {[istarget i?86-*-vxworks*]
    || [istarget mips*-*-vxworks*]
    || [istarget sh*-*-vxworks*]
    || [istarget sparc*-*-vxworks*]} {
    # The kernel strncmp doesn't perform unsigned comparisons.
    set torture_eval_before_execute {
	global compiler_conditional_xfail_data
	set compiler_conditional_xfail_data {
	    "The kernel strncmp doesn't perform unsigned comparisons."
	    { "*-*-*" }
	    {}
	    { "-mrtp" }
	}
    }
}
return 0
