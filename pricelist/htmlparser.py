#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

from urllib import quote, quote_plus
import re

def unescape(s):
	from htmllib import HTMLParser
	p = HTMLParser(None)
	p.save_bgn()
	p.feed(s)
	return p.save_end()

def find_closing(element, page, start=0):
	element = element.lower()
	page = page.lower()

	children = True
	while children:
		opening = page.find('<'+element+' ', start+1)
		closing = page.find('</'+element+'>', start+1)
		if closing < opening or opening == -1:		# no (more) children of the same class
			return closing

		start = find_closing(element, page, opening)
		if start == -1:					# end of page or page corrupted
			return start

def straddle(page, startkey):
	p = page.find(startkey)
	if p < 0:
		return '',page
	p += len(startkey)

	startkey = startkey.strip()
	end = startkey.find(' ')
	if end == -1:
		end = len(startkey)
	element = startkey[1:end]
	q = find_closing(element, page, p)

	return page[:p], page[q:]

def between(hay, before, after, occurence=1, include=False, include_before=False, include_after=False): # return substring from haystack between "before" and "after"
	haystack = str(hay)
	start = 0
	for i in range(1, occurence):
		start = haystack.find(before, start)+1
	p = haystack.find(before, start)
	if p < 0:
		return ''
	if (not include) and (not include_before):
		p += len(before)
	q = haystack.find(after, p)
	if q < 0:
		q = len(haystack)-1
	else:
		if include or include_after:
			q += len(after)
	return haystack[p:q]

def betweens(hay, before, after, include=False, include_before=False, include_after=False): # return substring from haystack between "before" and "after"
	results = []
	i = 1
	result = between(hay, before, after, i, include=include, include_before=include_before, include_after=include_after)
	while result != '':
		results.append(result)
		i += 1
		result = between(hay, before, after, i, include=include, include_before=include_before, include_after=include_after)
	return results

def getflashvar(haystack, key):
	return between(haystack, 'flashvars.'+key+'="', '"')

def getvalue( haystack, field ):				# return the value in haystack: '... field=value ...'
	lower = haystack.lower().replace(">", " ")
	p = lower.find( field+"=" )
	if p > -1:
		p += len( field )+1
	else:
		return ""
	if haystack[p] == '"':					# " ist Trennzeichen
		p += 1
		q = lower.find( '"', p )
	elif haystack[p] == "'":				# ' ist Trennzeichen
		p += 1
		q = lower.find( "'", p )
	else:							# Keine Gänsefüßchen, Leerzeichen trennt zum nächsten Feldnamen
		q = lower.find( " ", p )
	return haystack[p:q]

def convert_to_base(number, base):				# integer conversion
	currentbase = base
	while number >= currentbase:
		currentbase = currentbase * base
	result = ""
	while currentbase > 1:
		currentbase = int(currentbase/base)
		d = int(number/currentbase)
		number -= d*currentbase

	return result

def toString(number, base):
	if base <= 36:							# dictionary size: 36 characters
		if number < 10:
			return str(number)				# 0-9
		else:
			if number > 35:
				return toString(number/base, base)+toString(number % base, base)
			else:
				return chr(ord('a')+number-10)		# a-z
	elif base == 54:
		if number < 59-38+3:
			return chr(number+29)
		else:
			if number > 54:
				return toString(number/base, base)+toString(number % base, base)
			else:
				return chr(ord('a')+number-(59-38+3))

def e(c, base):
	if c < base:
		return ''
	else:
		if c % base > 35:
			return e(int(c/base)) + String.fromCharCode( (c % base)+29 )
			#	0-9
			#	a-z
			#	(){}
			#	'
			#	:.,
			#	/-
		else:
			return e(int(c/base)) + toString(c % base, 36)
			#	0-9
			#	a-z

ElementClassesOfInterest = { "input": ["type", "name", "id", "value"], "img": ["name", "id", "src"] }

class Element:							# Form method parse stores it's elements as array of Element classes
	def __init__(self, HTML):
		p = HTML.find("<")+1
		if p == -1:
			return None
		q = HTML.find(" ", p)
		self.Class = HTML[p:q].lower()
		if self.Class in ElementClassesOfInterest.keys():
			for field in ElementClassesOfInterest[ self.Class ]:
				self.__dict__[ field ] = getvalue( HTML, field )

class Form:							# Robot method parse() stores results as array of Form classes
	def __init__(self, HTML=None):
		if HTML is not None:
			self.parse( HTML )
		else:
			self.name = self.id = self.action = self.method = ""

	def parse(self, HTML):
		self.name   = getvalue( HTML, "name" )
		self.id	    = getvalue( HTML, "id" )
		self.action = getvalue( HTML, "action" )
		self.method = getvalue( HTML, "method" )

		lower = HTML.lower()	# mit ">"
		for Class in ElementClassesOfInterest.keys():
			self.__dict__[ Class ] = {}
			starter = "<"+Class+" "
			p = lower.find( starter )
			while ( p > -1 ):
				q = lower.find( ">", p )
				e = Element(HTML[p:q])
				if "name" in e.__dict__.keys():
					self.__dict__[ Class ][e.name] = e
				p = lower.find( starter, q )

	def POSTdict(self):
		result = {}
		for i in self.input.values():
			result[ i.name ] = quote_plus( i.value ) #.replace(" ", "+") 
		return result

	def POSTline(self):
		result = ""
		for i in self.input.values():
			result += i.name+"="+quote_plus( i.value )+"&"
		return result.rstrip("&")

class Eval:
	def __init__(self, source, debug=False):
		self.source = source
		self.debug = debug

	def __str__(self):
		return self.source

	# Algorithmus:
	# 6 Variablen werden an eine JavaScript-Dechiffrier-Funktion übergeben: P,A,C,K,E und D
	# P ist das Chiffre
	# A ist die Größe des Schlüssel-Wörterbuches
	# C ist die Nummer des ersten Schlüssels, der in Klartext umgewandelt wird
	# K ist das Klartext-Wörterbuch
	# E und D haben keine Bedeutung

	def unpack(self):
		# extract the JS function's arguments

		p = self.source.find("('", self.source.find("return p"))+2
		q = self.source.find("',", p)
		while self.source[q-1] == '\\':		# it's "\'," instead of "',", so we are still in the middle of P
			q = self.source.find("',", q+1)
		P = self.source[p:q]						# input
		if self.debug:
			print "cipher: "+P
		q += 2
		r = self.source.find(",", q)
		A = int(self.source[q:r])					# base to convert to
		if self.debug:
			print "base: "+str(A)
		if A > 36:
			A = 36							# JavaScript toString radix cannot be > 36

		r += 1
		s = self.source.find(",'", r)
		C = int(self.source[r:s])					# magic number
		if self.debug:
			print "number: "+str(C)
		s += 2
		t = self.source.find("'.split", s)
		K = self.source[s:t].split("|")					# replacement dictionary
		if self.debug:
			print "dict: "+str(K)
		return P,A,C,K

	def deobfuscate(self):
#		function(p,a,c,k,e,d) {
#			while(c--): 
#				if(k[c]):
#					p=p.replace(new RegExp('\\b'+c.toString(a)+'\\b','g'),k[c]);
#			return p
#			}
		code, base, number, dictionary = self.unpack()	# p,a,c,k

		while number >= 0:
			if number < len(dictionary):
				before = toString(number, base)
				after = dictionary[number]
				if self.debug:
					#print str(number)+' base '+str(base)+' = '+str(before)
					print "Replacing "+str(before)+" by "+str(after)+" ..."
				code = re.sub(r'\b'+before+r'\b', after, code)
				if self.debug:
					print code
			number -= 1
		return code

class HTML:
	def __init__(self, HTML, debug=False):
		self.string = HTML
		self.forms = None
		self.debug = debug

	def log(self, msg):
		if self.debug:
			print msg

	def __str__(self):
		return self.string

	def lower(self):
		return self.string.lower()

	def find(self, needle, start_at=0):
		return self.string.find(needle, start_at)

	def parse(self):
		if self.forms is not None:
			return
		self.forms = []

		self.log("Searching document for forms ...")
		lower = self.lower()
		p = lower.find("<form")						# find form
		while ( p > -1 ):
			if lower[p+5] == " " or lower[p+5] == ">":		# is it "<form " or "<form>" ?
				q = lower.find("</form>", p)+7			# find end of form
				if q == -1:
					q = len(lower)				# if end not found, copy until end of file
				self.forms.append( Form(self.string[p:q]) )
			else:
				q = p+1
			p = lower.find("<form", q)

		self.log("Found "+str(len(self.forms))+" forms.")

	def findForm(self, name=None, ID=None, action=None):			# return specified form
		if self.forms is None:
			self.parse()						# parse, if not parsed already
		if name is not None:
			for form in self.forms:
				if form.name == name:				# get form by name
					return form
		elif ID is not None:
			for form in self.forms:
				if form.id == ID:				# get form by ID
					return form
		elif action is not None:
			for form in self.forms:
				if form.action == action:			# get form by action
					return form
		elif len(self.forms) > 0:					# if no parameters are given, return the first form
			return self.forms[0]
		return None

	def findEval(self, occurence=1):
		return Eval("eval("+between(self.string, "eval(", "</script>", occurence).strip(), debug=False)

	def save(self, filename=None):
		if filename is None:
			filename = "robot.html"
		open(filename,"w").write(self.string)

