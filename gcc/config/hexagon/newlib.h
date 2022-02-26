/*-------------------------------------
Controlling the Compilation Driver, gcc
-------------------------------------*/

/* Link crt0_standalone first. */
#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0_standalone.o%s startmain.o%s crt0.o%s crti.o%s crtbegin.o%s"

#define HEXAGON_NEWLIB 1
