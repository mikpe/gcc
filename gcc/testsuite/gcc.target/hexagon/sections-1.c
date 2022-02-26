/* Check primitive types are placed in the correct section. */

/* { dg-do compile { target { ! *-*-linux* } } } */
/* { dg-options "" } */
/* { dg-skip-if "-G0 disables small data sections" { "*-*-*" } { "-G0" } { "" } } */

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.8.8,\"aws\",@nobits\n\t\.subsection\t-3\n\t\.comm\tsbss_ll,8,8,8\n" } } */
long long sbss_ll;

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.4.4,\"aws\",@nobits\n\t\.subsection\t-2\n\t\.comm\tsbss_l,4,4,4\n" } } */
long sbss_l;

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.2.2,\"aws\",@nobits\n\t\.subsection\t-1\n\t\.comm\tsbss_s,2,2,2\n" } } */
short sbss_s;

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.1.1,\"aws\",@nobits\n\t\.comm\tsbss_c,1,1,1\n" } } */
char sbss_c; 

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.8.8,\"aws\",@progbits\n\t\.subsection\t-3\n\t\.p2align 3\n\t\.type\tsdata_ll, @object\n\t\.size\tsdata_ll, 8\nsdata_ll:\n" } } */
long long sdata_ll = 1;

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.4.4,\"aws\",@progbits\n\t\.subsection\t-2\n\t\.p2align 2\n\t\.type\tsdata_l, @object\n\t\.size\tsdata_l, 4\nsdata_l:\n" } } */
long sdata_l = 1;

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.2.2,\"aws\",@progbits\n\t\.subsection\t-1\n\t\.p2align 1\n\t\.type\tsdata_s, @object\n\t\.size\tsdata_s, 2\nsdata_s:\n" } } */
short sdata_s = 1;

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.1.1,\"aws\",@progbits\n\t\.type\tsdata_c, @object\n\t\.size\tsdata_c, 1\nsdata_c:\n" } } */
char sdata_c = 1;
