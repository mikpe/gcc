load_lib target-supports.exp

if { ! [check_effective_target_nonlocal_goto] } {
    return 1
}

if [istarget "epiphany-*-*"] {
    # This test assumes the absence of struct padding.
    # to make this true for test3 struct A on epiphany would require
    # __attribute__((packed)) .
    return 1
}

set torture_eval_before_compile {
  if { [istarget "cdp1802-*-*"] && [string match {*-O3*} "$option"] } {
    # At -O3 the code is too large for the 16-bit address space.
    continue
  }
}    

return 0
