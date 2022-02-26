/* Check structures are placed in the correct section -- namley the
   correct section for the smallest primitive they contain. */

/* { dg-do compile { target { ! *-*-linux* } } } */
/* { dg-skip-if "-G0 disables small data sections" { "*-*-*" } { "-G0" } { "" } } */

struct s1 {
  char a;
  char b;
};

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.1.1,\"aws\",@progbits\n\t\.type\ts1_data, @object\n\t\.size\ts1_data, 2\ns1_data:" } } */
struct s1 s1_data = {1, 1};

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.1.1,\"aws\",@nobits\n\t\.comm\ts1_bss,2,1,1\n" } } */
struct s1 s1_bss;

struct s2 {
  short a;
  short b;
};

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.2.2,\"aws\",@progbits\n\t\.subsection\t-1\n\t\.p2align 1\n\t\.type\ts2_data, @object\n\t\.size\ts2_data, 4\ns2_data:" } } */
struct s2 s2_data = {1, 1};

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.2.2,\"aws\",@nobits\n\t\.subsection\t-1\n\t\.comm\ts2_bss,4,2,2\n" } } */
struct s2 s2_bss;

struct s3 {
  long a;
  long b;
};

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.4.4,\"aws\",@progbits\n\t\.subsection\t-2\n\t\.p2align 2\n\t\.type\ts3_data, @object\n\t\.size\ts3_data, 8\ns3_data:" } } */
struct s3 s3_data = {1, 1};

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.4.4,\"aws\",@nobits\n\t\.subsection\t-2\n\t\.comm\ts3_bss,8,4,4\n" } } */
struct s3 s3_bss;

struct s4 {
  long a;
  short b;
  char c;
};

/* { dg-final { scan-assembler "\t\.section\t\.sdata\.1.4,\"aws\",@progbits\n\t\.p2align 2\n\t\.type\ts4_data, @object\n\t\.size\ts4_data, 8\ns4_data:" } } */
struct s4 s4_data = {1, 1, 1};

/* { dg-final { scan-assembler "\t\.section\t\.sbss\.1.4,\"aws\",@nobits\n\t\.comm\ts4_bss,8,4,1\n" } } */
struct s4 s4_bss;

struct s5 {
  long a;
  struct {
    long a;
    short b;
    long c;
  } b;
};

/* { dg-final { scan-assembler "\t\.data\n\t\.p2align 2\n\t\.type\ts5_data, @object\n\t\.size\ts5_data, 16\ns5_data:" } } */
struct s5 s5_data = {1, {1, 1, 1}};

/* { dg-final { scan-assembler "\t\.section\t\.bss,\"aw\",@nobits\n\t\.comm\ts5_bss,16,4" } } */
struct s5 s5_bss;
