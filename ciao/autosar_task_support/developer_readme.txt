Content:
--------
This folder contains files to properly use the ciao task support. Reschedules
are initiated if we otherwise would active-wait. Use the ciao alarm/event support
to let the ipstack task sleep instead of active waiting for response packets.

Interrupt locking:
------------------
If you use AS:: methods (kernel context) you have to synchronize.
Because this is not done in aspects automatically, AS:: calls are not synchronized!
EnterLeave_Weaving_Static.ah uses the call advice, which doesn't match in advices.
Workaround: call enterKernel() and leaveKernel() explicit!
