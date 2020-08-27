# DNS-Amplification-Attack

### Use raw socket to create an IP spoofing packets
* Create a raw socket with UDP protocol.
* Fabricate the IP header.
* Fabricate the UDP header.
* Calculate the checksum over IP and UDP headers.
* Create DNS query in the UDP payload.

### Amplify DNS Attack
* Use EDNS to make the packet become much larger.
* Fabricate OPT structure in DNS additional records section

### Attack
```bash
$ su
$ make
$ ./dns victim_ip 7
```
### Result

Send a packet with 80 bytes and the victim would receive the packet in 1040 bytes!
