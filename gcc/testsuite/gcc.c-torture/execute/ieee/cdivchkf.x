if [istarget "avr-*-*"] {
    # Floating-point support is incomplete.
    return 1
}
if [istarget "cdp1802-*-*"] {
   # The code is too large for the address space
   return 1
}
return 0
