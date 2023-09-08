#!/usr/bin/perl
#
# run.pl -- a standalone perl scripts for testing the behavior of
# calling a program from within a perl script.
# This feature is periodically problematic with the TOPS20 gcc test
# scripts.
#

$ProgName = "run.pl";
#$RunExe = "/sys/exec.exe";
$RunExe = "/sys/sh.exe";

$runcmd = "$ARGV[0]";
$runcmd =~ s/\^/:</g;
$runcmd =~ s/@/>/g;

printf ("RUN COMMAND:\n%s\n", $runcmd);

$runcmd .= "\n";

if (!open(RUN_PIPE, "|" . $RunExe)) {
    printf ("%s: ERROR: Failed to open pipe to %s\n", $ProgName, $RunExe);
    exit 1;   # Exit code is presently ignored by topstale.
}
printf ("RUN_PIPE opened\n");
printf (RUN_PIPE "$runcmd");
printf ("command sent to RUN_PIPE\n");

#printf (RUN_PIPE "pop\n");
printf (RUN_PIPE "exit\n");
close(RUN_PIPE);
printf ("RUN_PIPE closed \n");

exit(0);   # Exit code is presently ignored by topstale.
printf ("shouldn't get here because it's after the exit(0)\n");




