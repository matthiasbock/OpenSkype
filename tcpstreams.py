#!/usr/bin/python

#
# Follow a TCP stream inside a PCAP dump
# by Matthias Bock, August 2012
#
# requires python-libpcap
#

#from pcap import pcapObject
import dpkt
from ethernet import Ethernet
from ip import IP, IP_PROTO_TCP
from tcp import TCP

class TCPStreams:
	def __init__(self, pcap_obj):

		# create an array of tcp streams: source IP:port, destination IP:port

		self.streams = []
		p.reset()
		def iterate(pktlen, pkt, timestamp):
			eth = Ethernet(pkt)
			if eth.type == IP:
				ip = IP(eth.data)
				if ip.proto == IP_PROTO_TCP:
					tcp = TCP(ip.data)
					f = {	'src': ip.src, 'srcport': tcp.srcport,
						'dst': ip.dst, 'dstport': tcp.dstport   )
					if not f in self.streams:
						self.streams.append(f)
		p.loop(0, iterate)

	def filter(self, dstport=None):

		# delete all array entries, that do not match the criteria

		for f in self.streams:
			if f['dstport'] != dstport:
				self.streams.drop(f)

	def __array__(self):
		return self.streams

class FollowTCPStream:
	def __init__(self, pcap_obj, streamlist):
		self.pcap_obj = pcap_obj
		self.streamlist = streamlist

	def loop(self, last_packet=0, outgoing, incoming):
		self.pcap_obj.reset()
		def iterate(pktlen, pkt, timestamp):
			eth = Ethernet(pkt)
			if eth.type == IP:
				ip = IP(eth.data)
				if ip.proto == IP_PROTO_TCP:
					tcp = TCP(ip.data)
					matches = False
					for stream in self.streamlist:
						if (ip.src == stream['src'] and tcp.srcport == stream['srcport']
						and ip.dst == stream['dst'] and tcp.dstport == stream['dstport'])
							outgoing(len(p.data), p.data, p.timestamp)
							break
						if (ip.src == stream['dst'] and tcp.srcport == stream['dstport']
						and ip.dst == stream['src'] and tcp.dstport == stream['srcport']):
							incoming(len(p.data), p.data, p.timestamp)
							break
		self.pcap_obj.loop(last_packet, iterate)

def TCPHeader(pkt):
        assert (data [12:14] == '\x08\x00') # IP
        assert (ord (data [14]) & 0xf0 == 0x40) # IPv4
        assert (data [23] == '\x06') # TCP
	...
	return pkt[...]

def TCPData(pkt):
        assert (data [12:14] == '\x08\x00') # IP
        assert (ord (data [14]) & 0xf0 == 0x40) # IPv4
        assert (data [23] == '\x06') # TCP
        hl  = 4 * (ord (data [14]) & 0xf) + 14
        tcp = data [hl:]
        thl = (ord (tcp [12]) & 0xf0) >> 2
        return tcp [thl:]

