load_lib target-supports.exp

if { [check_effective_target_int32plus] } {
	set additional_flags "-fsigned-bitfields"
	return 0
}

return 1;
