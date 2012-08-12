#!/usr/bin/python

def print_address(ip):
	n = []
	for i in range(4):
		n.append(str(ord(ip[i])))
	return '.'.join(n).rjust(4*3+3)

def long2hex(l):
	result = ''
	for i in range(4):
		result = hex(int(l & 0xFF))[2:4].zfill(2)+result
		l = (l >> 8) & 0xFFFFFF
	return result

def str2hex(s):
	return ''.join([hex(ord(s[i]))[2:].zfill(2) for i in range(len(s))])

def str2long(s):
	return ord(s[0]) * 0x01000000 + ord(s[1]) * 0x010000 + ord(s[2]) * 0x0100 + ord(s[3])

def hexstr2bytestr(h):
	result = ''
	for i in range(len(h)/2):
		result += chr(int(h[i*2]+h[i*2+1], 16))
	return result

def int2bin(i):
	mask = 0x1
	s = ''
	while mask <= i:
		if i & mask > 0:
			s = '1' + s
		else:
			s = '0' + s
		mask *= 2
	return s
