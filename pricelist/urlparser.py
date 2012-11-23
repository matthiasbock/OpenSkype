#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

HTTP	= "http://"
HTTPS	= "https://"

# splits an URL into Protocol (http://), Host (www.kernel.org) and Site (/sources/linux)

def splitURL(URL, default_protocol=HTTP, default_host="localhost", default_site="/"):

	Protocol = default_protocol					# determine protocol
	if URL[:8].lower() == HTTPS:
		Protocol = HTTPS
	elif URL[:7].lower() == HTTP:
		Protocol = HTTP

	if URL[:len(Protocol)] == Protocol:				# remove Protocol from URL
		URL = URL[len(Protocol):]

	s = URL.split("/")						# Host, Site
	
	Host = s[0] if len(s[0]) > 0 else default_host

	Site = "/"+"/".join(s[1:]) if len(s) > 1 else default_site

	return Protocol, Host, Site

