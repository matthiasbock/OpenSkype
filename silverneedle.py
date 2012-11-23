#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

from rc4 import RC4_Engine

from utils import *

#
# Ask for other nodes' IP
# p.61/98
#

src = chr(67)+chr(172)+chr(146)+chr(158)
dst = chr(172)+chr(16)+chr(15)+chr(2)
objectid = '\x3c\x66'
iv = '\x31\xEB\x8C\x94'
crc = '\x75\x01\x2A\xAF'
answear = '\xf5\x01~\xd1\xb0(\xa8\x03\xd1\xd9\x8d6\x97\xd6\x9e\xc0\x04<\x99\xf0\x0c\x14\x1d\xd6‘\xe2\xdc\xc0\xc3\x8d\xb4B\xa4\x9f\xd5\xbcK\x96\xccB\xaa\x17eBt8EA,K\xc2\xab\x04\x11\xf2\x1fR\x93lp.I\x96H\xd4=:\x06y\xfb'

rc4 = RC4_Engine(debug=True)
plaintext = rc4.decrypt(answear, src, dst, objectid, iv, crc)

print str2hex(plaintext)
