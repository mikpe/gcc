/* Check that unique section naming works. */

/* { dg-do compile { target { ! *-*-linux* } } } */
/* { dg-skip-if "-G0 disables small data sections" { "*-*-*" } { "-G0" } { "" } } */
/* { dg-options "-fdata-sections" } */
/* { dg-final { scan-assembler ".sdata.4.4.bar" } } */

long bar = 10;
