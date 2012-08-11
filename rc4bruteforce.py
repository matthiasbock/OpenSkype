#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *
import sys


class Rambo:
	def __init__(self, cipher_hex, crc):
		self.cipher = hexstr2bytestr(cipher_hex)
		self.crc = crc

	def crack(self, start=0x00000000L):
		finalseed = start

		last_write = 0
		crc_correct = False
		while not crc_correct:

			rc4context = RC4_Context()
			Skype_RC4_Expand_IV(finalseed, rc4context)
			plaintext = RC4_crypt(self.cipher, rc4context)
			crc = long2hex(skype_crc(plaintext))
			crc_correct = crc == self.crc
			if not crc_correct:
				finalseed += 1
				if finalseed-last_write > 100000:
					open('currentfinalseed','w').write(long2hex(finalseed))
					last_write = finalseed

		print long2hex(finalseed)+' - cracked'
		open('finalseed','w').write(long2hex(finalseed))



#bruteforce = Rambo(cipher_hex='ca5f4abe629bebf39df2152facca17', crc='b0a81c91') # PAYLOAD
bruteforce = Rambo(cipher_hex='dccb580bc50d5f944fbf65f5671dd2', crc='b0a81c91') # RESEND

bruteforce.crack(start=0x88888888) #start=0x768df200) # finalseed of the PAYLOAD

