# this test uses long long data type, which is not supported on the pdp10
# set torture_execute_xfail "*-*-*"
# return 1

# more examples of how .x files can be written
# mtc 11/30/2007


load_lib target-supports.exp

if { [check_effective_target_stdint_types] } {
	return 0
}

return 1;

