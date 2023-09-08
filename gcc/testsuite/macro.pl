#!/usr/bin/perl
#
# macro.pl -- called from topstale on TOPS20 to invoke the TOPS20 macro
# assembler via a pipeline, and pump standard output from macro back
# to topstale.
#
# This scheme is complicated due to characters-as-command-args
# limitations in the Perl V4 implementation on TOPS20.  See comments 
# in topstale for additional info.
#
# A string argument comes to this script encoded with the following
# character encoding scheme:
#     ':<' chars are encoded as '^'
#     '>' char is encoded as '@'
#     newline char is encoded as '#'
# All other chars are treated as literals.

$ProgName = "macro.pl";
$MacroExe = "/sys/macro.exe";

$macrocmd = "$ARGV[0]";
$macrocmd =~ s/\^/:</g;
$macrocmd =~ s/@/>/g;

printf ("MACRO COMMAND:\n%s\n", $macrocmd);

$macrocmd .= "\n\032";

if (!open(MACRO_PIPE, "|" . $MacroExe)) {
    printf ("%s: ERROR: Failed to open pipe to %s\n", $ProgName, $MacroExe);
    exit 1;   # Exit code is presently ignored by topstale.
}
printf (MACRO_PIPE "$macrocmd");
close(MACRO_PIPE);
exit(0);   # Exit code is presently ignored by topstale.
