#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *

shortkey = 0x10325434L

rc4context = RC4_Context()
Skype_RC4_Expand_IV(shortkey, rc4context)
#plaintext = RC4_crypt(self.cipher, rc4context)

longkey = str2hex(str(rc4context.s))
print str(len(longkey))
print longkey

