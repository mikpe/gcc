if [istarget "avr-*-*"] {
    # AVR doubles are floats
    return 1
}
if [istarget "cdp1802-*-*"] {
   # At -Og the code has too many local objects
   return 1
}
return 0
