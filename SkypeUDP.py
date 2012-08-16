# $Id$

# Skype's UDP Protocol

import dpkt

"""
name_to_code = {
    'Char'             : 'c',
    'Byte'             : 'b',
    'UnsignedByte'     : 'B',
    'Int'              : 'i',
    'UnsignedInt'      : 'I',
    'Short'            : 'h',
    'UnsignedShort'    : 'H',
    'Long'             : 'l',
    'UnsignedLong'     : 'L',
    'String'           : 's',  
    'PascalString'     : 'p',  
    'Pointer'          : 'P',
    'Float'            : 'f',
    'Double'           : 'd',
    'LongLong'         : 'q',
    'UnsignedLongLong' : 'Q',
    }
"""

class SkypeUDP(dpkt.Packet):
	__hdr__ =	(
			('objectid', '2s', '\x00' * 2),
			('type', 'B', 0),
			)

SKYPEUDP_TYPE_PAYLOAD		= 0x2
SKYPEUDP_TYPE_RESEND		= 0x3
SKYPEUDP_TYPE_CRCERR		= 0x5
SKYPEUDP_TYPE_CRCERR_RESEND	= 0x7
SKYPEUDP_TYPE_AUDIO		= 0xd
SKYPEUDP_TYPE_FRAGMENT		= 0xf
SKYPEUDP_TYPES_RECOGNIZED = [	SKYPEUDP_TYPE_PAYLOAD,
				SKYPEUDP_TYPE_RESEND,
				SKYPEUDP_TYPE_CRCERR,
				SKYPEUDP_TYPE_CRCERR_RESEND,
				SKYPEUDP_TYPE_AUDIO,
				SKYPEUDP_TYPE_FRAGMENT,
				]

# Object List starts with : 0x41 1 byte number of elements

class Payload(dpkt.Packet):
	__hdr__ =	(
			('iv', '4s', '\x00' * 4),
			('crc', '4s', '\x00' * 4),
			)

class Resend(dpkt.Packet):
	__hdr__ =	(
			('number', 'B', '\x01'),
			('ivseed', '4s', '\x00' * 4),
			('unknown', '4s', '\x00' * 4),
			('crc', '4s', '\x00' * 4),
			)

class CrcError(dpkt.Packet):
	__hdr__ =	(
			('yourip', '4s', '\x00' * 4),
			('ivseed', '4s', '\x00' * 4),
			)

class Fragment(dpkt.Packet):
	__hdr__ =	(
			('inreplyto', '2s', '\x00' * 2),
			('flag', 'B', 0x01),
			('magic', 'B', 0x02),
			('iv', '4s', '\x00' * 4),
			('crc', '4s', '\x00' * 4),
			)

