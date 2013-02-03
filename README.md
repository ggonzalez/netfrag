netfrag
=======

Reads data from an input socket and forwards data fragmented.

I need to quickly write to workaround an iOS UDP reassembling bug so I had to fix the upper limit for UDP packets.

Example:

Input Stream: TCP on port 4444
Output Stream: UDP max payload 2048 bytes forwarded to 192.168.168.1 port 1234

$ ./netfrag 4444 192.168.168.1 1234 2048
