#!/usr/bin/perl
#
# link.pl -- called from topstale on TOPS20 to invoke the TOPS20
# link'er via a pipeline, and pump standard output from link back
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

$ProgName = "link.pl";
$LinkExe = "/sys/link.exe";
$linkcmd = "$ARGV[0]";
$linkcmd =~ s/\^/:</g;
$linkcmd =~ s/@/>/g;
$linkcmd =~ s/\#/\n/g;

printf ("LINK COMMANDS:\n%s\n", $linkcmd);

if (!open(LINK_PIPE, "|" . $LinkExe)) {
    printf ("%s: ERROR: Failed to open pipe to %s\n", $ProgName, $LinkExe);
    exit 1;   # Exit code is presently ignored by topstale.
}
printf (LINK_PIPE "$linkcmd");
close(LINK_PIPE);
exit(0);   # Exit code is presently ignored by topstale.
