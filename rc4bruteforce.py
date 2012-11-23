#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *
import sys

class Rambo:
	def __init__(self, cipher_hex, crc):
		self.cipher = hexstr2bytestr(cipher_hex)
		self.crc = crc

	def crack(self, start=0x00000000, stop=0xFFFFFFFF, exit=True):
		shortkey = start

		last_key_written = 0
		crc_correct = False

		while (not crc_correct):
			rc4context = RC4_Context()
			Skype_RC4_Expand_IV(shortkey, rc4context)
			plaintext = RC4_crypt(self.cipher, rc4context)
			crc = long2hex(skype_crc(plaintext))
			crc_correct = crc == self.crc
			if not crc_correct:
				if shortkey < stop:
					shortkey += 1
					if shortkey-last_key_written > 50000:
						f = open('rc4bf-'+self.crc+'-'+sys.argv[1]+'.current','w')
						f.write(long2hex(shortkey))
						f.close()
						last_key_written = shortkey
						print long2hex(shortkey)
				else:
					f = open('rc4bf-'+self.crc+'-'+sys.argv[1]+'.current','w')
					f.write(long2hex(shortkey))
					f.close()
					print 'tried all keys without success. giving up ...'
					break
				
		if crc_correct: # cracked
			f = open('rc4bf-'+self.crc+'-'+sys.argv[1]+'.cracked','w')
			f.write('shortkey: '+long2hex(shortkey)+'\n')
			f.write('plaintext: '+str2hex(plaintext)+'\n')
			f.write('crc: '+crc)
			f.close()
			print long2hex(shortkey)+' - cracked'
			print '\tcrc:\t'+crc
			print '\tplaintext:\n\t\t'+str2hex(plaintext)
			if exit:
				sys.exit()
			return plaintext

if __name__ == '__main__':

#	bruteforce = Rambo(cipher_hex = 'ca5f4abe629bebf39df2152facca17', crc = 'b0a81c91') # PAYLOAD; correct shortkey=0x768df200
#	bruteforce = Rambo(cipher_hex = 'dccb580bc50d5f944fbf65f5671dd2', crc = 'b0a81c91') # RESEND; false positive @ 0x202ec8b5

	bruteforce = Rambo(cipher_hex = 'f5017ed1b028a803d1d98d3697d69ec0043c99f00c141dd6e28098e2dcc0c38db442a49fd5bc4b96cc42aa176542743845412c4bc2ab0411f21f52936c702e499648d43d3a0679fb', crc = '75012AAF')

	if sys.argv[1] == '0':		# 0-4
		bruteforce.crack(start = 0x29f80655, stop = 0x40000000)
		bruteforce.crack(start = 0x691baa72, stop = 0x80000000)
	elif sys.argv[1] == '1':	# 4-8
		bruteforce.crack(start = 0x850ab7ab, stop = 0xa0000000)
		bruteforce.crack(start = 0xa4f9c74d, stop = 0xffffffff)
#	elif sys.argv[1] == '2':	# 8-a
#	elif sys.argv[1] == '3':	# a-f

