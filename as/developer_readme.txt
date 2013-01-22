Content:
--------
This folder contains files to interact with the underlying OS. If you want to port
the IPStack to another OS, you have to provide all functionallity of these files.

Interrupt locking:
------------------
If you use AS:: methods (kernel context) you have to synchronize.
Because this is not done in aspects automatically, AS:: calls are not synchronized!
EnterLeave_Weaving_Static.ah uses the call advice, which doesn't match in advices.
Workaround: call enterKernel() and leaveKernel() explicit!
