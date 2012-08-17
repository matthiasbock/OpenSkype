#!/usr/bin/python

#
# Skype uses arithmetic compression:
# the Huffman algorithm
#
# Hint: the picture, presented in "Silver needle in the Skype",
# does not represent the frequency dictionary of "ACAB"
#

def getfrequencies(s, debug=False):
	frequencies = {}
	for c in s:
		if c in frequencies.keys():
			frequencies[c] += 1
		else:
			frequencies[c] = 1
	if debug:
		print '\tsymbol frequencies: '+str(frequencies)
	return frequencies

def frequencies2dictionary(length, frequencies, debug=False):
	keys = sorted(frequencies.keys())
	interval = 1.0/length
	dictionary = {}
	start = 0.0
	for k in keys:
		step = frequencies[k]*interval
		dictionary[k] = [start, start+step]
		start += step
	if debug:
		print '\tsymbol dictionary: '+str(dictionary)
	return dictionary

def makedictionary(s, debug=False):
	return frequencies2dictionary( len(s), getfrequencies(s, debug), debug )

#
# Example usage:
#  interval = [0.5, 0.75]
#  subinterval = [0.4, 0.6]
#  return [0.5+0.4*0.25, 0.5+0.6*0.25]
#
def subdivide(interval, subinterval):
	start	= interval[0]
	stop	= interval[1]
	width	= stop-start
	istart	= subinterval[0]
	istop	= subinterval[1]
#	iwidth	= istop-istart
	return [ start+(istart*width), start+(istop*width)  ]

#
# compress string to real
#
def compress(s, debug=False):
	l = len(s)
	if debug:
		print 'compressing "'+s+'" ...'
		print '\tstring length: '+str(l)
	d = makedictionary(s, debug)
	r = [0, 1]
	for c in s:
		r = subdivide(r, d[c])
		if debug:
			print '\t'+c+': '+str(r)
	return r[0],d,l

#
# decompress real to string
#
def decompress(r, dictionary, length, debug=False):
	if debug:
		print 'decompressing "'+str(r)+'" ...'
		print '\tstring length: '+str(l)
		print '\tsymbol dictionary: '+str(dictionary)
	s = ''
	interval = [0, 1]
	from copy import deepcopy
	applied_dictionary = deepcopy(dictionary)
	for i in range(length):
		for k in applied_dictionary.keys():	# which is the next char ?
			subinterval = applied_dictionary[k]
			if r >= subinterval[0] and (r < subinterval[1] or (r == 1 and subinterval[1] == 1)):
				s += k
				break
		if debug:
			print '\t'+str(r)+' is in '+str(subinterval)+' -> '+k
		for k in dictionary.keys():
			applied_dictionary[k] = subdivide(subinterval, dictionary[k])
	return s

if __name__ == '__main__':
	import sys
	try:
		r,d,l = compress(sys.argv[1], debug=True)
	except:
		r,d,l = compress('ACABCACA', debug=True)

	decompress(r, d, l, debug=True)

