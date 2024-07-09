# freebsd sets up the fpu with a different precision control which causes
# this test to "fail".
if { [istarget "i?86-*-freebsd*\[123\]\.*"] } {
	set torture_execute_xfail "i?86-*-freebsd*"
}
if [istarget "cdp1802-*-*"] {
   # At -O0 the code is too large for the address space
   return 1
}
return 0
