#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt

#from ports import *
kazaa = 1214

from pcap import pcapObject
from dpkt.ethernet import *
from dpkt.ip import *
from dpkt.udp import *
from skypeudp import *

def print_address(ip):
	n = []
	for i in range(4):
		n.append(str(ord(ip[i])))
	return '.'.join(n).rjust(4*3+3)

def long2hex(l):
	if l == 0:
		return '00'
	result = ''
	while l > 0:
		result = hex(int(l & 0xFF))[2:4].zfill(2)+result
		l = (l >> 8) & 0xFFFFFF
	return result

def str2hex(s):
	return ''.join([hex(ord(s[i]))[2:].zfill(2) for i in range(len(s))])

def str2long(s):
	return ord(s[0]) * 0x01000000 + ord(s[1]) * 0x010000 + ord(s[2]) * 0x0100 + ord(s[3])

counter = 1
ExternalIP = '\x00' * 4

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
				print '#'+str(counter).ljust(4)+': Skype-UDP (topic '+topic+') from '+print_address(ip.src)+':'+str(udp.sport).ljust(5)+' to '+print_address(ip.dst)+':'+str(udp.dport).ljust(5)+'  ',
				if t == SKYPEUDP_TYPE_PAYLOAD:
					payload = Payload(header.data)

#					print
#					plaintext = FluxCapacitor.decrypt_udp(str(ip), src=ExternalIP)
#					print 'plaintext: '+str2hex(plaintext)

					src = ip.src
					if src[1:3] == chr(192)+chr(168):
						src = ExternalIP
					dst = ip.dst
					if dst[1:3] == chr(192)+chr(168):
						dst = ExternalIP

					# bitwise xor of seed from ip addresses, topic and initial value
					seed = Seed(src, dst, header.topic)
#					print 'seed: '+long2hex(seed)
					seed = seed ^ str2long(payload.iv)
#					print 'seed ^ iv: '+long2hex(seed)

					rc4context = RC4_Context()
					Skype_RC4_Expand_IV(seed, rc4context)
					plaintext = RC4_crypt(payload.data, rc4context)
					calc_crc = long2hex(skype_crc(plaintext))	# long
					pkt_crc = str2hex(payload.crc)			# string
					okay = 'correct'
					if calc_crc != pkt_crc:
						okay = 'incorrect, decryption failed'

					print 'PAYLOAD (iv='+str2hex(payload.iv)+', CRC='+str2hex(payload.crc)+' '+okay+'), +'+str(len(payload.data))+' bytes.'
					print '\tplaintext: '+str2hex(plaintext)
				elif t == SKYPEUDP_TYPE_NAT_INFO:
					nat = NAT_Info(header.data)
					print 'NAT present, your IP address: '+print_address(nat.yourip)+', error code? '+str2hex(nat.seed)
					if (ExternalIP[0] == '\x00' or ip.dst[0:2] == chr(192)+chr(168)) and ExternalIP != nat.yourip:
						ExternalIP = nat.yourip
						print '\texternal IP is now set to '+print_address(ExternalIP)
				elif t == SKYPEUDP_TYPE_NAT_REPEAT:
					nat = NAT_Info(header.data)
					print 'NAT present - please repeat, your IP address: '+print_address(nat.yourip)+', error code? '+str2hex(nat.seed)
					if (ExternalIP[0] == '\x00' or ip.dst[0:2] == chr(192)+chr(168)) and ExternalIP != nat.yourip:
						ExternalIP = nat.yourip
						print '\texternal IP is now set to '+print_address(ExternalIP)
				elif t == SKYPEUDP_TYPE_RESEND:
					resend = Resend(header.data)
					print 'RESEND, retry no. '+str(resend.number)+', error code?='+str2hex(resend.seed)+', iv='+str2hex(resend.iv)+', CRC='+str2hex(resend.crc)+', +'+str(len(resend.data))+' bytes.'
#					print '\t'+str2hex(plaintext[:7])+'...'
				elif t == SKYPEUDP_TYPE_AUDIO:
					print 'AUDIO STREAM: '+str(len(header.data))+' bytes.'
				elif t == SKYPEUDP_TYPE_UNKNOWN:
					print 'Unknown: '+str(len(header.data))+' bytes.'
				else:
					print 'UNKNOWN ('+hex(t)+'), '+str(len(header.data))+' bytes follow'
	counter += 1

import sys

try:
	fname = sys.argv[1]
except:
	fname = 'SkypeIRC.cap'

p = pcapObject()
p.open_offline (fname)
p.loop (0, iterate)

