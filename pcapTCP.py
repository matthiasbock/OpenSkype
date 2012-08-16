#!/usr/bin/python

import sys

from pcap import pcapObject

from dpkt.ethernet import Ethernet
from dpkt.ip import IP
from dpkt.tcp import TCP

from tcpstreams import TCPStreams, FollowTCPStream
from rc4 import RC4_Engine
from utils import *

#-----------------------------------------------------------------
# SkypeTCP dissection
#-----------------------------------------------------------------

try:
	fname = sys.argv[1]
except:
	fname = 'SkypeIRC.cap'

#p = pcapObject()
#p.open_offline(fname)
streams = TCPStreams(fname).filter(dport=40022)
print str(len(streams))+' streams filtered.'
s = streams[0]
stream = FollowTCPStream(fname, s)
print 'Following the chat between '+print_address(s['src'])+':'+str(s['sport'])+' and '+print_address(s['dst'])+':'+str(s['dport'])

def outgoing(pktlen, pkt, timestamp):
	eth = Ethernet(pkt)
	ip = IP(str(eth.data))
	tcp = TCP(str(ip.data))
	print ' -> '+str2hex(tcp.data)

def incoming(pktlen, pkt, timestamp):
	eth = Ethernet(pkt)
	ip = IP(str(eth.data))
	tcp = TCP(str(ip.data))
	print ' <- '+str2hex(tcp.data)
	
stream.loop(0, outgoing, incoming)

