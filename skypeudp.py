# $Id$

# Skype's UDP Protocol

import dpkt

class SkypeUDP(dpkt.Packet):
	__hdr__ =	(
			('seq', 'H', 0xdead),
			('type', 'B', 0),
			)

SKYPEUDP_PAYLOAD	= 0x2
SKYPEUDP_RESEND		= 0x3
SKYPEUDP_CRCERROR	= 0x7

class Payload(dpkt.Packet):
	__hdr__ =	(
			('crc', '4s', '\x00' * 4),
			)

	def decrypt():
		return "not supported yet"

class Resend(dpkt.Packet):
	__hdr__ =	(
			('no', 'B', '\x01'),
			('errcode', '4s', '\x00' * 4),
			)

class CrcError(dpkt.Packet):
	__hdr__ =	(
			('yourip', '4s', '\x00' * 4),
			('errcode', '4s', '\x00' * 4),
			)

