# this test requires run time library routine memcpy which pdp10 doesn't
# support yet
# this compiles but doesn't link because of the missing routine.
# set to skip the execution step so it doesn't show up as a failure
# I don't know how to mark that the execution phase is expected to fail.
set torture_eval_before_execute continue
return 0
