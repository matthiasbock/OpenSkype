#!/usr/bin/python

#
# Follow a TCP stream inside a PCAP dump
# by Matthias Bock, August 2012
#
# requires python-libpcap
#

from pcap import pcapObject
import dpkt
from dpkt.ethernet import Ethernet, ETH_TYPE_IP
from dpkt.ip import IP, IP_PROTO_TCP
from dpkt.tcp import TCP

class TCPStreams:
	def __init__(self, filename):

		# create an array of tcp streams: source IP:port, destination IP:port

		self.filename = filename
		self.streams = []

		p = pcapObject()
		p.open_offline(self.filename)
		def iterate(pktlen, pkt, timestamp):
			eth = Ethernet(pkt)
			if eth.type == ETH_TYPE_IP:
				ip = IP(str(eth.data))
				if ip.p == IP_PROTO_TCP:
					tcp = TCP(str(ip.data))
					f = {	'src': ip.src, 'sport': tcp.sport,
						'dst': ip.dst, 'dport': tcp.dport   }
					if not f in self.streams:
						self.streams.append(f)
		p.loop(0, iterate)

	def filter(self, dport=None):
		result = []
		for f in self.streams:
			if f['dport'] == dport:
				result.append(f)
		return result

	def __array__(self):
		return self.streams

class FollowTCPStream:
	def __init__(self, filename, stream):
		self.filename = filename
		self.stream = stream

	def loop(self, last_packet=0, outgoing=None, incoming=None):
		p = pcapObject()
		p.open_offline(self.filename)
		def iterate(pktlen, pkt, timestamp):
			eth = Ethernet(pkt)
			if eth.type == ETH_TYPE_IP:
				ip = IP(str(eth.data))
				if ip.p == IP_PROTO_TCP:
					tcp = TCP(str(ip.data))
					if len(tcp.data) > 0:
						if (outgoing is not None
						and ip.src == self.stream['src'] and tcp.sport == self.stream['sport']
						and ip.dst == self.stream['dst'] and tcp.dport == self.stream['dport']):
							outgoing(pktlen, pkt, timestamp)
						if (incoming is not None
						and ip.src == self.stream['dst'] and tcp.sport == self.stream['dport']
						and ip.dst == self.stream['src'] and tcp.dport == self.stream['sport']):
							incoming(pktlen, pkt, timestamp)
		p.loop(last_packet, iterate)

# Ethernet Header:	14 bytes
# IP Header:		20 bytes
# TCP Header:		32 bytes

