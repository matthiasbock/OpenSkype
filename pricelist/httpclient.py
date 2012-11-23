#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

import httplib
from urlparser import splitURL, HTTP, HTTPS
from htmlparser import HTML, Form
from datetime import datetime
import socket
import sys

GET	= "GET"
POST	= "POST"

def decode(page):
	page = page.decode('iso-8859-15')
	i = 0
	while i < len(page):
		try:
			str(page[i])
			i += 1
		except:
			if len(page) > i+1:
				page = page[:i]+page[i+1:]
			else:
				page = page[:i]
	return page

class HttpClient:
	def __init__(self, debug=False):								# defaults
		self.Connection = None
		self.ConnectionTimeout = 3 # seconds
		self.Protocol = HTTP
		self.Host = "localhost"
		self.Agent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)"
		self.SendReferer = True
		self.Site = "/"
		self.SendCookies = True
		self.Cookies = {}
		self.Headers = []
		self.Page = None
		self.parsed = False
		self.Forms = []
		self.AutoSave = False
		self.print_debug = debug
		self.debug("Robot ready.")

	def debug(self, msg):
		if self.print_debug:
			print msg

	def connect(self):										# connect to remote host
		self.HaveReferer = False
		self.debug("Connecting to "+self.Host+" ...")
		if self.Protocol == HTTP:
			self.Connection = httplib.HTTPConnection( self.Host )
		elif self.Protocol == HTTPS:
			self.Connection = httplib.HTTPSConnection( self.Host )				# Warning: All certificates are accepted !
		self.last_request = datetime.now()

	def request(self, Method, URL, Variables={}, AdditionalHeaders={}, SendReferer=None, SendCookies=None, FollowRedirects=True):		# returns response.status code

		Method = Method.upper()											# check Method
		if not Method in [GET,POST]:
			self.debug("Invalid method: "+Method)
			return

		if self.Page is not None:
			del self.Page
		Protocol, Host, Site = splitURL(URL, default_protocol=self.Protocol, default_host=self.Host, default_site=self.Site)
		self.debug("Request: "+Method+" "+Protocol+Host+Site)

		if self.Connection is not None:		# if it's None, we are disconnected and can safely skip this check
			if ( self.Host != Host ) or ( self.Protocol != Protocol ):
					# connected, but to the different host or protocol
					self.debug("Connected to "+self.Protocol+self.Host+", but request is for "+Protocol+Host+".")
					self.disconnect()

		if self.Connection is None:		# disconnected, either anyway or by the previous check
			self.Protocol = Protocol
			self.Host = Host
			self.Site = "/"
			self.connect()

#		delta = (datetime.now() - self.last_request).seconds
#		if delta > self.ConnectionTimeout:
#			self.debug("Connection timed out "+str(delta)+" seconds ago.")
#			self.disconnect()
#			self.connect()

		additional_headers = {"Host":self.Host, "User-Agent":self.Agent, "Accept-Encoding":"identity", "Connection":"keep-alive"}
			# at least Accept-Encoding:identity is added by httplib anyway, but is nevertheless shown here for completeness
		if SendReferer is None:
			SendReferer = self.SendReferer								# send a referer ?
		if SendReferer and self.HaveReferer:
			additional_headers["Referer"] = self.Protocol+self.Host+self.Site
		additional_headers.update(AdditionalHeaders)
		if 'Referer' in additional_headers.keys():
			if SendReferer:
				self.debug("\tReferer: "+additional_headers["Referer"])
			else:
				del additional_headers["Referer"]
		if not 'Referer' in additional_headers.keys():
			self.debug("\tNo referer.")
		self.HaveReferer = True

		if SendCookies is None:
			SendCookies = self.SendCookies								# send cookies ?
		if SendCookies and ( self.Cookies != {} ):
			c = ""
			for key in self.Cookies:
				c += key+"="+self.Cookies[key]+"; "
			additional_headers["Cookie"] = c
			self.debug("\tSending cookie.")
		else:
			self.debug("\tNo cookie.")

		Parameters = ""											# send form variables ?
		for key in Variables:
			Parameters += str(key)+"="+str(Variables[key])+"&"
		Parameters = Parameters.rstrip("&")
		if Variables != {} and ('Content-Type' not in additional_headers.keys()):
			additional_headers["Content-Type"] = "application/x-www-form-urlencoded"

		self.debug("\tRequest: "+Method+" "+Site+" "+Parameters+" "+str(additional_headers)+'"')		# HTTP REQUEST
		try:
			self.Connection.request(Method, Site, Parameters, additional_headers)
		except socket.error:
			errno, errstr = sys.exc_info()[:2]
			if errno == 110:
				self.debug("socket.error: [Errno 110] Connection timed out. Reconnecting ...")
				self.disconnect()
				self.connect()
				self.Connection.request(Method, Site, Parameters, additional_headers)
			elif errno == -2:
				self.debug("Exception. Probably host name could not be resolved.")
				raise
			else:
				self.debug("Uncaught socket error")
				raise
		self.last_request = datetime.now()
#		try:
		response = self.Connection.getresponse()
#		except httplib.BadStatusLine:
#			self.connect()
		self.debug("\t"+str(response.status)+" "+response.reason)
		self.Headers = {}
		for Tuple in response.getheaders():
			self.Headers[Tuple[0]] = Tuple[1]
		self.Page = HTML(decode(response.read()))  # remember: you must read every response in order to be ready to receive the next one!
		if self.AutoSave:
			self.savepage()

		if response.status == 200: # 200 OK
			if SendReferer:
				self.Site = Site

			Cookie = response.getheader("Set-Cookie")		# process received cookies
			if SendCookies and Cookie != None and Cookie != "":
				key = "expires="				# "expires" value contains comma. must be removed before splitting.
				p = Cookie.find(key)
				while p > -1:
					q = Cookie.find(";",p)
					if q == -1:				# "expires" is located at the end of the cookie: no semicolon
						_Cookie = Cookie[:p]
					else:
						_Cookie = Cookie[:p] + Cookie[q+1:]
					Cookie = _Cookie
					p = Cookie.find(key)
				Cookie = Cookie.split(",")
				for C in Cookie:
					s = C.split(";")[0].split("=")		# save only until first semicolon, discard path=...; domain=...;
					key = s[0].lstrip(" ")
					value = "=".join(s[1:])
					if value == 'deleted':
						if key in self.Cookies.keys():
							del self.Cookies[ key ]
						self.debug("\tCookie deleted: "+key+" = "+value)
					else:
						self.Cookies[ key ] = value
						self.debug("\tCookie received: "+key+" = "+value)

			return response.status

		elif response.status in [301, 302]: # Redirects: 301 Moved Permanently, 302 Found
			location = response.getheader("location")						# Redirection
			if ( location != None ) and ( location != "" ):
				if FollowRedirects:
					self.debug("\tRedirected to "+location)					# follow ...
					return self.request( GET, location, {}, {}, SendReferer, SendCookies, FollowRedirects )
				else:
					self.debug("\tRedirect to "+location+" ignored.")
					return location
			else:
				return response.status

		else:	# not 200 OK or 302 Redirect
			return response.status

	def GET(self, URL, Variables={}, AdditionalHeaders={}):
		return self.request(GET, URL, Variables, AdditionalHeaders)

	def POST(self, URL, Variables={}, AdditionalHeaders={}):
		return self.request(POST, URL, Variables, AdditionalHeaders)

	def XMLHttpRequest(self, URL, Variables={}, AdditionalHeaders={}):		# emulate a JavaScript XMLHttpRequest
		AdditionalHeaders['X-Requested-With'] = 'XMLHttpRequest'
		return self.request(GET, URL, Variables, AdditionalHeaders, SendReferer=False)

	def ajax(self, method, URL, Variables={}, AdditionalHeaders={}):		# emulate ajax XMLHttpRequest
		AdditionalHeaders['X-Requested-With'] = 'XMLHttpRequest'
		if method == GET:
			return self.request(GET, URL, Variables, AdditionalHeaders, SendReferer=False)
		elif method == POST:
			return self.request(POST, URL, Variables, AdditionalHeaders, SendReferer=False)

	def submit(self, form):								# submit form
		self.request( form.method, form.action, form.POSTdict() )

	def export_cookie_as_wget_arguments(self):
		result = ""
		for key in self.Cookies.keys():
			result += '--header="Cookie: '+key+'='+self.Cookies[key]+'" '
		return result

	def disconnect(self):
		try:
			signal.alarm(0)			# disable the connection timeout alarm
		except:
			pass
		if self.Connection is not None:
			self.Connection.close()
			self.debug("Disconnected.")
		self.Connection = None			# __init__ would be too much, e.g. if one host redirects to another host we may need the referer

	def __del__(self):
		try:
			self.disconnect()
			self.debug("Robot destroyed.")
		except:
			pass

