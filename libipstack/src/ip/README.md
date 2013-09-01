# Direcory "ip"
This directory contains common IP related code, not specific to IPv4 or IPv6. There is
common checksumming code, checksum offloading and
you'll find the single/dual-stack implementation in the subdirectory **dual_single_stack**.

## Management Task
The IP management task is a collection of background related tasks you need for not user specific traffic generated
by the IP-Stack. This can be time related tasks like refreshing router table entries or ip specific tasks
like icmpv4/v6 responses/requests.