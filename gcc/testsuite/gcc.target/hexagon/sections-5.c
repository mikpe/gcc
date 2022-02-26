/* Check section sorting works correctly with attributes. */

/* { dg-do compile { target { ! *-*-linux* } } } */
/* { dg-skip-if "-G0 disables small data sections" { "*-*-*" } { "-G0" } { "" } } */

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.1\.8,\"aws\",@nobits\n\t\.comm\tc_bss_8,1,8,1\n" } } */
char c_bss_8 __attribute__((aligned(8)));

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.2\.4,\"aws\",@nobits\n\t\.subsection\t-1\n\t\.comm\ts_bss_4,2,4,2\n" } } */
short s_bss_4 __attribute__((aligned(4)));

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.4\.4,\"aws\",@progbits\n\t\.subsection\t-2\n\t\.p2align 2\n\t\.type\tl_nocommon, @object\n\t\.size\tl_nocommon, 4\nl_nocommon:\n\t\.zero\t4" } } */
long l_nocommon __attribute__((nocommon));


/* { dg-final { scan-assembler "\.section\t\.sdata\.1\.1,\"aw\",@progbits\n\t\.subsection\t-2\n\t\.p2align 2\n\t\.type\tstruct_section, @object\n\t\.size\tstruct_section, 8\nstruct_section:\n\t\.word\t1\n\t\.word\t1" } } */
struct {
  long l1;
  long l2;
} struct_section __attribute__((section(".sdata.1.1"))) = { 1, 1 };
