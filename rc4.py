#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *

class RC4_Engine:
	def __init__(self):
		self.ExternalIP		= '\x00\x00\x00\x00'
		self.print_cipher	= True
		self.print_seeding	= True
		self.print_plaintext	= True

	def decrypt(self, cipher, src, dst, topic, iv, crc):
		if self.print_cipher:
			print '\tcipher:\n\t\t'+str2hex(cipher)

		correct = False

		test = -1
		test_sources =	[
				'\x00\x00\x00\x00',
				'\xFF\xFF\xFF\xFF',
				]
#		for j in range(2):
#			for i in range(255):
#				test_sources.append(chr(192)+chr(168)+chr(j+1)+chr(i+1))

		while not correct:
			print '\tsrc ip: '+print_address(src),

			seed = Seed(src, dst, topic)
			if self.print_seeding:
				print '\tseed: '+long2hex(seed)
			seed = seed ^ str2long(iv)			# XOR
			if self.print_seeding:
				print '\tseed ^ iv: '+long2hex(seed)

			rc4context = RC4_Context()
			Skype_RC4_Expand_IV(seed, rc4context)
			plaintext = RC4_crypt(cipher, rc4context)
			calc_crc = long2hex(skype_crc(plaintext))	# long
			pkt_crc = str2hex(crc)				# string

			correct = calc_crc == pkt_crc
			okay = 'correct'
			if not correct:
				okay = 'incorrect, decryption failed'
				test += 1
				if test >= len(test_sources):
					correct = True
				else:
					src = test_sources[test]
			print '\t'+okay

		if self.print_plaintext and calc_crc == pkt_crc:
			print '\tplaintext:\n\t\t'+str2hex(plaintext)

