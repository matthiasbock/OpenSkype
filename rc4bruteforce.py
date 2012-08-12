#!/usr/bin/python

from FluxCapacitor import Seed, Skype_RC4_Expand_IV, skype_crc, RC4_Context, RC4_crypt
from utils import *
import sys

class Rambo:
	def __init__(self, cipher_hex, crc):
		self.cipher = hexstr2bytestr(cipher_hex)
		self.crc = crc

	def crack(self, start=0x00000000, stop=0xFFFFFFFF):
		finalseed = start

		last_write = 0
		crc_correct = False
		give_up = False

		while (not crc_correct) and (not give_up):
			rc4context = RC4_Context()
			Skype_RC4_Expand_IV(finalseed, rc4context)
			plaintext = RC4_crypt(self.cipher, rc4context)
			crc = long2hex(skype_crc(plaintext))
			crc_correct = crc == self.crc
			if not crc_correct:
				if finalseed < stop:
					finalseed += 1
					if finalseed-last_write > 10000:
						open('currentfinalseed','w').write(long2hex(finalseed))
						print long2hex(finalseed)
						last_write = finalseed
				else:
					open('currentfinalseed','w').write(long2hex(finalseed))
					print 'tried all keys without success. giving up ...'
					give_up = True
				
		if not give_up:
			print long2hex(finalseed)+' - cracked'
			print '\tcrc:\t'+crc
			print '\tplaintext:\n\t\t'+str2hex(plaintext)
#			open('finalseed','w').write(long2hex(finalseed))
			return plaintext

if __name__ == '__main__':

	#bruteforce = Rambo(cipher_hex='ca5f4abe629bebf39df2152facca17', crc='b0a81c91') # PAYLOAD; correct finalseed=0x768df200
#	bruteforce = Rambo(cipher_hex='dccb580bc50d5f944fbf65f5671dd2', crc='b0a81c91') # RESEND; false positive @ 0x202ec8b5
#	bruteforce = Rambo(cipher_hex = '2f81fcdca0200813e4bfbaf96216', crc = '3913b8a9')
#	bruteforce = Rambo(cipher_hex = 'a8605bd1f1fb842ec42bc770f2a45d6f9eae088f5394728c1e41954307b3b1af351cfd36ee49177d27f303fa8ca6b1f386f3063ce467524ebb3d34ebb40c9edf62b00d2a085cf0178aeab5621e010524dd02dbb1d7dec3672ad604bb379299533643d2c71ce5372921c52117cd131d1a5a3cbf43f835acecdb3715d7d39e72afcb1c1f4def4ee05bad30a532a71f7c76b3afc04006bb3579fd3293f738d0108122ec87c304ca35a36ddafa9dce320b8cb85e4e0c380a68554eae977ba9709c47730e9331a4cf72a9405ac6a251938a87f312abfddf753090698093dbdc4283880d7e7b02348550652d390c0324a3e422258ed0f3df900fe4fe8d471bd614bd84ab97dbab73361ca3fcf9e99bd09f98dd77e1089b5392a2b1aacdac2b71639626bbd2106b229a96096713d8a14dec959d8af1c37527d45c00de00736695a1e98438e679eda7d2df364b7fd8aa48e6e50f9de63c5bf05e6e551b692a30d2bec27c12b590e90630489d27e70feea1a09eee49dbfe4c29bac7cbe6c3fccf41c9ff524024521630205b4ad35580a03db17a297a4ec1f43d0e555e093cb98887bb670b551668c57bcd58a7af1ece6fa012c752c8d0ac89db6729ad29f5f1360c0085cfc551eac6f32337dd8398de04ef09d0517e3bd294327db206bde0a3f0d9848234176dbd86eb82da50877e61d283d2c719e89465e19508ff73b9fc54904f28291ab68c706e1c480594ba630cef9ae57cd5e5707ee7014a3f92eb427eaab2121b7c4acb667685b9126823a6d5ec71b18d53be9ba29a683f03ad2603a2ae9a50e217792f2d105b580a5c29241833cdf1d5780ef0faba9adf1881d58d1828e8b30cc5be2fd1ca7cccbd8da62ca17027843ed82f92211b220a82cdacb3cc3fa42cfd0d285ae2e83b26e36a8438720528cb5d717f4d8402beb9e91f17c57a235d1daa1bc32a3863ae0c6e3465226a62d36dbf2cdf93b812befa180a4e332bcbaba931622d3dbd2ff83043c418c50034967443ef63688cea569ae0685ededc34d8555fcdbf0b6596c56975f7cd3699b1fccfb25820bbf82f5066f511f2ca2ab4ab827690bdb071fd17d13ecc68b815aec5ffcd93a506920e1e36b25bd3e23c45520a263d1f8530140293bd7b13eea2893fcc9c74bf87bff44f3d98c7a234740d5a5da6f861936f16333ac307ea82edfa4cb96acdcf512a96b6f514dd22eb408d9d2ee552e371f606e6194d14bcb092bbcbd3e0dc56b07da3140937b22f11380e4d84e4e3774c6dd3c6704f31c21042fa2892fab9c7fdf0f274a5d5746cbc1560a5cdfc77fcd9f2324d7b12ba4536b9225df17c45e3f418f763398948e303878eb6ed19e37f4324943a03553b9363bd308b83b978d9bdc15bea48340c1ea46b01aaf2e055254138b0b09046bf25dad119444c54cd3d04739d843573cce1e9559d20b85bc99b74d6f63ba286f16c548d30f8078a3511370805289661388b0f7ea69ea96f043015844fe211a6edd3930f8fa053cb787f3e870eb764dc800bcae2be18a42f2efdade4f18bb05a57d605a6aea1e8b177a273430dc95622427dc2852ed296643c2b412031352f6a516cb8219c237d09f84ded1b47f1d10baaf7cf4d62f60235e2672d47da9b93c89eb41eeec6269eb89b1a72665176251ce71002d05eb93c4aeac370aa1d7bea7d42a1a013a5455296553db589891827b9d0dd9fda228b7c0ca363c730887df555fa28bab781f55c5373670b3545bb8f855a3ee68f60d0276e2c53ec9413a05496035a96ea6d229e2d99b03a9513dd9bd8798d5b8dff29f2d9661081421523226b9ef6f84e1ef6599b8b1fcc12018fefbe432111bdb7fbe3245f1456387616c689bfd50d23d7e4a7ac1620c64129c9a87fbbcc8d781f8b9a700ded021f61fae37cf5bb17ebedb136d0d7ba2d3399c30dc1f06319a02f8b11b44067b2cc828851dce8a78c84c7f8400085c05ca100175641385b596c11df5fad9782cf962e8ecf3426524cadc7a5aaa1e3c86ec25c333df9e38207d0747f6ed0a35b2ee6b1b69b144459b884fd30ca453185fab97c6ee67ecc43dc5deb62b5f7d750bfee77e539601674434179c557194efcbdfebb31c2d0bc51e1ff75f64acf49732c7b1b55caffb4989d21d5e9e023a6815ca395fc6a01e2e5548f69aa795dba7b49012ba2d592901150b47042430ec52b04b4b47e944a9ab90e67523406c8a5a637ca704fcc733c78f4ba6edae86bfe614f20ceff3205d88049d7c94b98d4664a8ee9bda489c041afca693876816daab8b5c62223e88a05b4cda4abfb4b89a57ab6985d86a28d2aca37fe1f87d620b989fae38d39c71ea6e7e0ba3f')

#	if sys.argv[1] == '0':
#		bruteforce.crack(start=0x0796769c, stop=0x20000000)
#	elif sys.argv[1] == '1':
#		bruteforce.crack(start=0x24628a18, stop=0x40000000)
#	else:
#		bruteforce.crack(start=0xec1e9911)

	bruteforce.crack(start = 0x66900000)

