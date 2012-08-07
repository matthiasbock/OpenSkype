# $Id$

# Skype's UDP Protocol

import dpkt

class SkypeUDP(dpkt.Packet):
	__hdr__ =	(
			('topic', 'H', 0xdead),
			('type', 'B', 0),
			)

SKYPEUDP_TYPE_PAYLOAD	= 0x2
SKYPEUDP_TYPE_RESEND		= 0x3
SKYPEUDP_TYPE_CRCERROR	= 0x7

class Payload(dpkt.Packet):
	__hdr__ =	(
			('iv', '4s', '\x00' * 4),
			('crc', '4s', '\x00' * 4),
			)

	def decrypt():
		return "not supported yet"

class Resend(dpkt.Packet):
	__hdr__ =	(
			('number', 'B', '\x01'),
			('seed', '4s', '\x00' * 4),
			('iv', '4s', '\x00' * 4),
			('crc', '4s', '\x00' * 4),
			)

class CrcError(dpkt.Packet):
	__hdr__ =	(
			('yourip', '4s', '\x00' * 4),
			('seed', '4s', '\x00' * 4),
			)

