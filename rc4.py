#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *

class RC4_Engine:
	def __init__(self):
		self.ExternalIP		= '\x00\x00\x00\x00'
		self.print_cipher	= False
		self.print_seeding	= False
		self.print_plaintext	= True

	def decrypt(self, cipher, src, dst, topic, iv, crc):
		if self.print_cipher:
			print '\tcipher:\n\t\t'+str2hex(cipher)

		test_sources =	[
				src,
				'\x00\x00\x00\x00',
				'\xFF\xFF\xFF\xFF',
				'\xec\x1e\x28\xd4',
				'\x8d\x14\x3f\x1c',
				chr(141)+chr(20)+chr(63)+chr(28),
				chr(111)+chr(221)+chr(77)+chr(155),
				]

		for j in range(2):
			for i in range(255):
				test_sources.append(chr(192)+chr(168)+chr(j+1)+chr(i+1))

		test_dests =	[
				dst,
				'\x00\x00\x00\x00',
				'\xFF\xFF\xFF\xFF',
				'\xec\x1e\x28\xd4',
				'\x8d\x14\x3f\x1c',
				chr(141)+chr(20)+chr(63)+chr(28),
				chr(111)+chr(221)+chr(77)+chr(155),
				]

#		for j in range(2):
#			for i in range(255):
#				test_dests.append(chr(192)+chr(168)+chr(j+1)+chr(i+1))

		correct = False

		for src in test_sources:
			for dst in test_dests:
				if self.print_seeding:
					print '\tsrc ip: '+print_address(src)+'\tdst ip: '+print_address(dst),

				seed = Seed(src, dst, topic)
				if self.print_seeding:
					print '\tseed: '+long2hex(seed),
				seed = seed ^ str2long(iv)			# XOR
				if self.print_seeding:
					print '\t\tseed ^ iv: '+long2hex(seed)

				rc4context = RC4_Context()
				Skype_RC4_Expand_IV(seed, rc4context)
				plaintext = RC4_crypt(cipher, rc4context)
				calc_crc = long2hex(skype_crc(plaintext))	# long
				pkt_crc = str2hex(crc)				# string

				correct = calc_crc == pkt_crc
				if correct:
					print '\tcrc correct'
					break
			if correct:
				break

		if self.print_plaintext and calc_crc == pkt_crc:
			print '\tplaintext:\n\t\t'+str2hex(plaintext)

		if not correct:
			print '\tdecryption failed'
