#!/usr/bin/python

from httpclient import HttpClient

country = 'DE'
currency = 'EUR'
language = 'de'

http = HttpClient()
http.GET('http://pricelist.skype.com/destinations/1.0/'+country+'/'+currency+'/'+language+'/xml/')

print str(http.Page)
