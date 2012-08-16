#!/usr/bin/python

import sys
from struct import unpack

from pcap import pcapObject

from dpkt.ethernet import Ethernet
from dpkt.ip import IP
from dpkt.tcp import TCP

from tcpstreams import TCPStreams, FollowTCPStream
from utils import *

from FluxCapacitor import RC4_Context, Skype_RC4_Expand_IV, RC4_crypt

#-----------------------------------------------------------------
# SkypeTCP dissection
#-----------------------------------------------------------------

try:
	fname = sys.argv[1]
except:
	fname = 'SkypeIRC.cap'

#p = pcapObject()
#p.open_offline(fname)
streams = TCPStreams(fname).filter(dport=57322) #40022)
print str(len(streams))+' streams filtered.'
s = streams[0]
stream = FollowTCPStream(fname, s)
print 'Following the chat between '+print_address(s['src'])+':'+str(s['sport'])+' and '+print_address(s['dst'])+':'+str(s['dport'])

first_out = True
seed_out = None
rc4_out = RC4_Context()

def outgoing(pktlen, pkt, timestamp):
	eth = Ethernet(pkt)
	ip = IP(str(eth.data))
	tcp = TCP(str(ip.data))
	print str(len(tcp.data)).zfill(2)+' bytes -> ',

	global first_out
	global seed_out
	global rc4_out
	if first_out:
		s = tcp.data[:4]
		seed_out = ord(s[0])*0x01000000 + ord(s[1])*0x010000 + ord(s[2])*0x0100 + ord(s[3])*0x01
		Skype_RC4_Expand_IV(seed_out, rc4_out)
		plaintext = RC4_crypt(tcp.data[4:14], rc4_out)
		print '\t'+str2hex(plaintext)
		rc4_out = RC4_Context()
		if len(tcp.data) > 14:
			Skype_RC4_Expand_IV(seed_in, rc4_out)
			plaintext = RC4_crypt(str(tcp.data[14:]), rc4_out)
			print '\t'+str2hex(plaintext)
		first_out = False
	else:
		Skype_RC4_Expand_IV(seed_in, rc4_out)
		plaintext = RC4_crypt(str(tcp.data), rc4_in)
		print '\t'+str2hex(plaintext)

first_in = True
seed_in = None
rc4_in = RC4_Context()

def incoming(pktlen, pkt, timestamp):
	eth = Ethernet(pkt)
	ip = IP(str(eth.data))
	tcp = TCP(str(ip.data))
	print str(len(tcp.data)).zfill(2)+' bytes <- ',

	global first_in
	global seed_in
	global rc4_in
	if first_in:
		s = tcp.data[:4]
		seed_in = ord(s[0])*0x01000000 + ord(s[1])*0x010000 + ord(s[2])*0x0100 + ord(s[3])*0x01
		Skype_RC4_Expand_IV(seed_in, rc4_in)
		plaintext = RC4_crypt(tcp.data[4:14], rc4_in)
		print '\t'+str2hex(plaintext)
		rc4_in = RC4_Context()
		if len(tcp.data) > 14:
			Skype_RC4_Expand_IV(seed_in, rc4_in)
			plaintext = RC4_crypt(str(tcp.data[14:]), rc4_in)
			print '\t'+str2hex(plaintext)
		first_in = False
	else:
		Skype_RC4_Expand_IV(seed_in, rc4_in)
		plaintext = RC4_crypt(str(tcp.data), rc4_in)
		print '\t'+str2hex(plaintext)
	
stream.loop(0, outgoing, incoming)

