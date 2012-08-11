#!/usr/bin/python

from rc4 import RC4_Engine
from pcap import pcapObject
from dpkt.ethernet import *
from dpkt.ip import *
from dpkt.udp import *
from skypeudp import *
from utils import *

#from ports import *
kazaa = 1214

print_cipher	= True
print_plaintext	= True
print_seeding	= True

counter = 1
rc4 = RC4_Engine()

def iterate(pktlen, data, timestamp):
	global counter
	global ExternalIP
	pkt = Ethernet(data)
	if pkt.type == ETH_TYPE_IP:
		ip = pkt.data
		if ip.p == IP_PROTO_UDP:
			udp = ip.data
#			if ord(udp.data[2]) & 0x0F in SKYPEUDP_TYPES_RECOGNIZED:
			if len(udp.data) > 3 and udp.sport not in [53, 137, 138, 5351, 5353, 1900, kazaa] and udp.dport not in [53, 137, 138, 5351, 5353, 1900, kazaa]:
				header = SkypeUDP(udp.data)
				topic = hex(ord(header.topic[0]))[2:].zfill(2) + hex(ord(header.topic[1]))[2:].zfill(2) + hex(header.type >> 4)[2:]
				t = header.type & 0x0F
				print '-'*100
				print '#'+str(counter).ljust(4)+':\tSkype-UDP (topic '+topic+') from '+print_address(ip.src)+':'+str(udp.sport).ljust(5)+' to '+print_address(ip.dst)+':'+str(udp.dport).ljust(5)

				if t == SKYPEUDP_TYPE_PAYLOAD:
					payload = Payload(header.data)

					plaintext = rc4.decrypt(payload.data, ip.src, ip.dst, header.topic, payload.iv, payload.crc)	# decrypt

					print '\tPAYLOAD: iv='+str2hex(payload.iv)+', CRC='+str2hex(payload.crc)+', +'+str(len(payload.data))+' bytes.'

				elif t == SKYPEUDP_TYPE_CRCERR:
					nat = CrcError(header.data)

					if ip.dst[0:2] == chr(192)+chr(168) or ip.dst[0:2] == rc4.ExternalIP[0:2]:	# error received, not sent
						rc4.updateIP(nat.yourip)

					print '\tCRC error: your IP address: '+print_address(nat.yourip)+', a public address?='+print_address(nat.seed)

				elif t == SKYPEUDP_TYPE_NAT_REPEAT:
					nat = CrcError(header.data)

					if ip.dst[0:2] == chr(192)+chr(168) or ip.dst[0:2] == rc4.ExternalIP[0:2]:	# error received, not sent
						rc4.updateIP(nat.yourip)
					
					print '\tCRC error: please repeat; your IP address: '+print_address(nat.yourip)+', a public address?='+print_address(nat.seed)

				elif t == SKYPEUDP_TYPE_RESEND:
					resend = Resend(header.data)

					plaintext = rc4.decrypt(resend.data, ip.src, ip.dst, header.topic, resend.iv, resend.crc)	# decrypt

					print '\tRESEND, retry no. '+str(resend.number)+', error code?='+str2hex(resend.seed)+', iv='+str2hex(resend.iv)+', CRC='+str2hex(resend.crc)+', +'+str(len(resend.data))+' bytes.'

				elif t == SKYPEUDP_TYPE_AUDIO:
					print 'AUDIO STREAM: '+str(len(header.data))+' bytes.'

				elif t == SKYPEUDP_TYPE_FRAGMENT:
					print '\tFragment: '+str(len(header.data))+' bytes.'

				else:
					print 'UNKNOWN ('+hex(t)+'), '+str(len(header.data))+' bytes follow'
				print
	counter += 1

import sys

try:
	fname = sys.argv[1]
except:
	fname = 'SkypeIRC.cap'

p = pcapObject()
p.open_offline (fname)
p.loop (0, iterate)

