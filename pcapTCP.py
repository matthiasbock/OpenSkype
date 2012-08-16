#!/usr/bin/python

from pcap import pcapObject
from dpkt.ethernet import *
from dpkt.ip import *
from dpkt.udp import *
from utils import *

from tcpstreams import TCPStreams, FollowTCPStream, TCPHeader, TCPData
from rc4 import RC4_Engine

#-----------------------------------------------------------------
# SkypeTCP dissection
#-----------------------------------------------------------------

stream = FollowTCPStream(p, TCPStreams(p).filter(dstport=33033) )

def outgoing(pktlen, pkt, timestamp):
	tcp = TCPHeader(pkt)
	data = TCPData(pkt)
#	...

def incoming(pktlen, pkt, timestamp):
	tcp = TCPHeader(pkt)
	data = TCPData(pkt)
#	...
	
stream.loop(0, outgoing, incoming)

