/*-------------------------------------
Controlling the Compilation Driver, gcc
-------------------------------------*/

#undef SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef TARGET_VERSION
#define TARGET_VERSION fputs (" (Hexagon GNU/Linux)", stderr);

#define MD_UNWIND_SUPPORT "config/hexagon/linux-unwind.h"

/* Use the generic pre-processor defines */
#define TARGET_OS_CPP_BUILTINS() LINUX_TARGET_OS_CPP_BUILTINS()

#define CPP_SPEC "%{posix:-D_POSIX_SOURCE} %{pthread:-D_REENTRANT}"

#define GLIBC_DYNAMIC_LINKER "/lib/ld.so.1"

/* Copied from config/svr4.h and modified to forward
   the -G option's argument, and not forward the -b option to the linker. */
#undef	LINK_SPEC
/* FIXME: This CROSS_COMPILE test does not work, so has been hardwired.  */
#if 1 || defined(CROSS_COMPILE)
#define LINK_SPEC "%{march=*} \
                   %{h*} %{v:-V} \
		   %{shared:-shared -G -dy} \
		   %{symbolic:-Bsymbolic -G -dy} \
                   %{G*} \
       %{!shared: \
         %{!static: \
       %{rdynamic:-export-dynamic} \
       -dynamic-linker " LINUX_DYNAMIC_LINKER "} \
       %{static:-dn -Bstatic}} \
		   %{YP,*} \
		   %{Qy:} %{!Qn:-Qy}"
#else /* !CROSS_COMPILE */
#define LINK_SPEC "%{march=*} \
                   %{h*} %{v:-V} \
		   %{shared:-shared -G -dy} \
		   %{symbolic:-Bsymbolic -G -dy} \
                   %{G*} \
       %{!shared: \
         %{!static: \
       %{rdynamic:-export-dynamic} \
       -dynamic-linker " LINUX_DYNAMIC_LINKER "} \
       %{static:-dn -Bstatic}} \
		   %{YP,*} \
		   %{!YP,*:%{p:-Y P,/usr/ccs/lib/libp:/usr/lib/libp:/usr/ccs/lib:/usr/lib} \
		    %{!p:-Y P,/usr/ccs/lib:/usr/lib}} \
		   %{Qy:} %{!Qn:-Qy}"
#endif /* !CROSS_COMPILE */

#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC \
  "%{march=hexagonv4: ; \
     march=hexagonv5:/v5}"

#undef HEXAGON_ABI_TABLE_DEFAULT_INDEX
#define HEXAGON_ABI_TABLE_DEFAULT_INDEX HEXAGON_ABI_LINUX

/* Select a format to encode pointers in exception handling data.  CODE
   is 0 for data, 1 for code labels, 2 for function pointers.  GLOBAL is
   true if the symbol may be affected by dynamic relocations.
 */
#undef ASM_PREFERRED_EH_DATA_FORMAT
#define ASM_PREFERRED_EH_DATA_FORMAT(CODE, GLOBAL) \
  ((flag_pic  ?  \
    (((GLOBAL) ? DW_EH_PE_indirect : 0) | DW_EH_PE_pcrel) \
       : DW_EH_PE_absptr) | \
  ((CODE) ? 0 : DW_EH_PE_sdata4))

/*-----------------------------
Implementing the Varargs Macros
-----------------------------*/
/* For Linux, our ABI handles varargs differently */
#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS hexagon_setup_incoming_varargs

#undef TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START hexagon_va_start
