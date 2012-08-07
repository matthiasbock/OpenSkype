#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, Skype_Decryptor, skype_crc

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
FluxCapacitor = Skype_Decryptor()
#d = FluxCapacitor.decrypt_udp (sky, '\0\0\0\0')



from pcap import pcapObject
from dpkt.ethernet import *
from dpkt.ip import *
from dpkt.udp import *
from skypeudp import *

LocalSkypePort = 35990   # 18555
ExternalIP = '\x00' * 4
#ExternalIP = ''.join([chr(int(n)) for n in '86.128.245.115'.split('.')])

def print_address(ip):
	n = []
	for i in range(4):
		n.append(str(ord(ip[i])))
	return '.'.join(n)

def longhex(l):
	return ''.join([hex(ord(l[i]))[2:].zfill(2) for i in range(len(l))])

counter = 1
def iterate(pktlen, data, timestamp):
	global counter
	global ExternalIP
	pkt = Ethernet(data)
	if pkt.type == ETH_TYPE_IP:
		ip = pkt.data
		if ip.p == IP_PROTO_UDP:
			udp = ip.data
			if udp.sport == LocalSkypePort or udp.dport == LocalSkypePort:
				header = SkypeUDP(udp.data)
				topic = hex(header.topic >> 8)[2:].zfill(2) + hex(header.topic & 0xFF)[2:].zfill(2) + hex(header.type >> 4)[2:]
				t = header.type & 0x0F
				print '#'+str(counter)+': Skype-UDP (topic '+topic+') from '+print_address(ip.src)+' to '+print_address(ip.dst)+': ',
				if t == SKYPEUDP_TYPE_PAYLOAD:
					payload = Payload(header.data)

#					seed = Seed(ip.src, ip.dst, [chr(header.topic[i]) for i in range(2)])
#					print seed
					plaintext = FluxCapacitor.decrypt_udp(str(ip), src=ExternalIP)
#					crc = skype_crc (plaintext)
#					okay = str(crc == payload.crc)
#					print 'payload: iv='+longhex(payload.iv)+', CRC='+longhex(payload.crc)+' ('+okay+'), +'+str(len(payload.data))+' bytes. decrypted message:'

					print 'payload: iv='+longhex(payload.iv)+', CRC='+longhex(payload.crc)+', +'+str(len(payload.data))+' bytes. decrypted message:'
					if len(plaintext) < 15:
						print '\t'+longhex(plaintext)
					else:
						print '\t'+longhex(plaintext[:15])+'...'
				elif t == SKYPEUDP_TYPE_CRCERROR:
					error = CrcError(header.data)
					ExternalIP = error.yourip
					print 'crc error, your IP address: '+print_address(error.yourip)+', error code '+longhex(error.seed)
				elif t == SKYPEUDP_TYPE_RESEND:
					resend = Resend(header.data)
					print 'resend, retry no. '+str(resend.number)+', seed='+longhex(resend.seed)+', iv='+longhex(resend.iv)+', CRC='+longhex(resend.crc)+', +'+str(len(resend.data))+' bytes. decrypted message:'
#					print '\t'+longhex(plaintext[:7])+'...'
				else:
					print 'unknown ('+hex(t)+'), '+str(len(header.data))+' bytes follow'
	counter += 1

import sys

try:
	fname = sys.argv[1]
except:
	fname = 'SkypeIRC.cap'

p = pcapObject()
p.open_offline (fname)
p.loop (220, iterate)

