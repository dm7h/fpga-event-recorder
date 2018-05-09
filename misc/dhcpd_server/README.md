* configure interface with static ip, eg:
'sudo ifconfig eth0 10.0.0.254 up'

* dhcpd.conf -> /etc/
* start dhcpd, eg:
'sudo systemctl start dhcpd4@eth0'

* ssh to pi
'ssh pi@10.0.0.1'