#!/usr/bin/python

#
# Skype uses arithmetic compression:
# the Huffman algorithm
#
# implementation of the algorithm described in "Silver needle in the Skype"
#

def makedictionary(s, debug=False):
	frequency = {}
	for c in s:
		if c in frequency.keys():
			frequency[c] += 1
		else:
			frequency[c] = 1
	if debug:
		print '\tsymbol frequencies: '+str(frequency)

	keys = sorted(frequency.keys())
	interval = 1.0/len(s)
	dictionary = {}
	start = 0.0
	for k in keys:
		step = frequency[k]*interval
		dictionary[k] = [start, start+step]
		start += step
	if debug:
		print '\tsymbol interval dictionary: '+str(dictionary)

	return dictionary

def subdivide(r, interval):
	start	= r[0]
	stop	= r[1]
	width	= stop-start
	istart	= interval[0]
	istop	= interval[1]
	iwidth	= istop-istart
	return [ start+(width*istart), start+(width*istop)  ]

def compress(s, debug=False):
	if debug:
		print 'compressing "'+s+'" ...'
		print '\tstring length: '+str(len(s))
	d = makedictionary(s, debug)
	r = [0, 1]
	for c in s:
		r = subdivide(r, d[c])
		if debug:
			print '\t'+c+': '+str(r)
	return r

if __name__ == '__main__':
	import sys
	try:
		compress(sys.argv[1], debug=True)
	except:
		compress('ACABCACA', debug=True)

