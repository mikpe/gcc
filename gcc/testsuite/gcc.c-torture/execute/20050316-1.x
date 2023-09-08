#load_lib target-supports.exp
#
#if { [check_effective_target_int16] } {
#	return 1
#}

#return 0;

# this test uses long long data type, which is not supported on the pdp10
set torture_execute_xfail "*-*-*"
return 0

