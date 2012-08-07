#!/usr/bin/python

from FluxCapacitor import Skype_Decryptor

#
# input a packet, starting with IP,
# cut the 14 bytes of Ethernet header
#

# Silver Needle example p. 41 is obviously a packet *before* Skype
# knows its external IP address, so the external IP used is 0.0.0.0
# the 172.16 address is RFC1918
# This is the same as Vanilla Skype part 1 p. 49
#sky =	'4500002e000440004011eb75'
#	'ac1048831862425008032053'
#	'001a219c7f4e02118ac037fc'
#	'95755e5eb9817a8efa81'.decode('hex')
#Flux_Capacitor = Skype_Decryptor ()
#d = Flux_Capacitor.decrypt_udp (sky, '\0\0\0\0')



from pcap import pcapObject
from dpkt.ethernet import *
from dpkt.ip import *
from dpkt.udp import *
from skypeudp import *

def print_address(ip):
	n = []
	for i in range(4):
		n.append(str(ord(ip[i])))
	return '.'.join(n)

def longhex(l):
	return ''.join([hex(ord(l[i]))[2:] for i in range(4)])

def iterate(pktlen, data, timestamp):
	pkt = Ethernet(data)
	if pkt.type == ETH_TYPE_IP:
		ip = pkt.data
		if ip.p == IP_PROTO_UDP:
			udp = ip.data
			if udp.sport == 18555 or udp.dport == 18555:
				print 'Skype-UDP from '+print_address(ip.src)+' to '+print_address(ip.dst)+': ',
				header = SkypeUDP(udp.data)
				t = header.type & 0x0F
				if t == SKYPEUDP_PAYLOAD:
					payload = Payload(header.data)
					print 'payload: '+str(len(payload.data))+' bytes'
				elif t == SKYPEUDP_CRCERROR:
					error = CrcError(header.data)
					print 'crc error, your IP address: '+print_address(error.yourip)+', error code '+longhex(error.errcode)+', payload: '+str(len(error.data))+' bytes'
				elif t == SKYPEUDP_RESEND:
					resend = Resend(header.data)
					print 'resend, retry no. '+str(resend.no)+', error code '+longhex(resend.errcode)
				else:
					print 'unknown ('+hex(t)+'), '+str(len(header.data))+' bytes follow'

p = pcapObject()
p.open_offline ('on-off 2012-08-07.pcap')
p.loop (0, iterate)

