#!/usr/bin/python
# Copyright (C) 2010 Dr. Ralf Schlatterbeck Open Source Consulting.
# Reichergasse 131, A-3411 Weidling.
# Web: http://www.runtux.com Email: office@runtux.com
# All rights reserved
# ****************************************************************************
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Note that the shared lib we're calling here is probably not GPL
# compatible. Use at your own risk.

# Wrapper for the recently revealed skype obfuscation layer from 
# http://cryptolib.com/ciphers/skype/ -- see blog article
# http://www.enrupt.com/index.php/2010/07/07/skype-biggest-secret-revealed

# To run this you need the ctypes library for python (included in recent
# python releases) and the libpcap wrapper for python (debian package
# python-libpcap)

from struct   import pack, unpack
#from binascii import crc32 # same as zlib
from zlib     import crc32
from ctypes   import CDLL, c_uint8, c_uint32, c_char_p, c_void_p, c_char
from ctypes   import POINTER, pointer, Structure

# Set LD_LIBRARY_PATH appropriately if this fails for you:
dll = CDLL ("./skype_rc4.so")

class RC4_Context (Structure) :
    """ RC4_Context used internally by the Skype_RC4_Expand_IV (which
        initializes the structure) and RC4_crypt routines.
    """
    _fields_ = (('i', c_uint8), ('j', c_uint8), ('s', c_uint8 * 256))
# end class RC4_Context

dll.RC4_crypt.restype  = None
dll.RC4_crypt.argtypes = \
    (c_char_p, c_uint32, POINTER (RC4_Context), c_uint32)
dll.Skype_RC4_Expand_IV.restype  = None
dll.Skype_RC4_Expand_IV.argtypes = \
    (c_uint32, c_void_p, POINTER (RC4_Context), c_uint32, c_uint32)

def Skype_RC4_Expand_IV (iv, rc4_context, iv2 = '', flags = 0) :
    """ Main RC4 IV expansion function, matching Skype parameters, with
        optional IV2 for the latest DH-384 based protocol
        This is a convenience wrapper, note that the parameters have
        been reordered to reflect the case where we don't want to
        specify iv2 (in fact the publicly available info currently
        doesn't tell us how to use the new DH-384 based protocol
        mentioned in the original docs, if you know more, please contact
        me at rsc@runtux.com)
    """
    return dll.Skype_RC4_Expand_IV (iv, iv2, rc4_context, flags, len (iv2))
# end def Skype_RC4_Expand_IV

def RC4_crypt (buffer, rc4_context, test = 0) :
    """ RC4 encrypt/decrypt (test=1 leaves rc4 context unaltered)
        This is a convenience wrapper.
    """

    buflen = len (buffer)
    b = (c_char * buflen)()
    for n, s in enumerate (buffer) :
        b [n] = s
    dll.RC4_crypt (b, buflen, rc4_context, test)
    return b [0:buflen]
# end def RC4_crypt

def ascii (s) :
    if len (repr (s)) > 3 :
        return '.'
    return s
# end def ascii

def dumphex (s) :
    r = []
    for x in xrange (len (s) / 16 + 1) :
        hex  = '%-48s' % ' '.join ("%02x" % ord (k) for k in s [x*16:(x+1)*16])
        char = '%-16s' % ''.join (ascii (k) for k in s [x*16:(x+1)*16])
        r.append ('  '.join ((hex, char)))
    return '\n'.join (r)
# end def dumphex

def skype_crc (s, seed = 0xFFFFFFFF) :
    """ Emulate crc function as used by skype -- the ethernet crc32
        (also used in pkzip and others) xors the result with 0xFFFFFFFF
        so we have to reverse that
        >>> x = crc32 ('abcd')
        >>> x = crc32 ('fghi', x)
        >>> x & 0xFFFFFFFF
        1292354512L
        >>> x = crc32 ('abcdfghi')
        >>> x & 0xFFFFFFFF
        1292354512L
        >>> x = emulated_crc ('abcd')
        >>> x
        310194926L
        >>> 0x127D32EEL
        310194926L
        >>> x = emulated_crc ('fghi', x)
        >>> x
        3002612783L
        >>> 0xB2F83C2FL
        3002612783L
        >>> x = emulated_crc ('abcdfghi')
        >>> x
        3002612783L
        >>> x = emulated_crc ('\xff\xf0\xf1\xf2')
        >>> x
        3949860366L
        >>> s = ''.join (chr (x) for x in xrange (0x80, 0x100))
        >>> x = emulated_crc (s)
        >>> x
        3115716450L
    """
    return (crc32 (s, seed ^ 0xFFFFFFFF) & 0xFFFFFFFF) ^ 0xFFFFFFFF
# end def emulated_crc

class Skype_Decryptor (object) :
    def __init__ (self, initial_data = None, iv = None) :
        """ Initialize from initial tcp packet or from given iv """
        self.restart = False
        if initial_data :
            self.init_tcp (initial_data)
        else :
            self.iv      = iv
            self.restart = True
    # end def __init__

    def init_tcp (self, sky) :
        """ Dissect start of TCP connection. Silver Needle tells us that
            a TCP stream starts with the skype IV followed by 10 bytes
            which should decrypt to 00 01 00 00 00 01 00 00 00 01
        """
        assert (len (sky) >= 14)
        #print dumphex (sky [:4])
        # For TCP Skype seems to put the IV into the TCP stream as the
        # first 4 bytes. We unpack as network byte order (!)
        iv  = unpack ('!L', sky [:4]) [0]
        #print "%08x" % iv
        rc4 = RC4_Context ()
        #print dumphex (sky [4:])
        Skype_RC4_Expand_IV (iv, rc4)
        r   = RC4_crypt (sky [4:], rc4, test = 1)
        assert (r.startswith ('\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01'))
        #print dumphex (r)
        self.rc4 = rc4
        self.iv  = iv
    # end def init_tcp

    def decrypt_tcp (self, data, restart = False) :
        """ Decrypt next TCP packet. Note that restart should not be
            necessary -- we decrypt the initial 10 byte sequence with
            test=1 parameter for RC4_crypt, so the rc4 internal state is
            not updated.
            We do not know yet if both sides of the TCP connection use
            the same key devived from the same iv or if different keys
            (how would these be exchanged?) are used in each direction.
            Probably the same key is used in both directions set up with
            the same iv. That means we need another instance of this
            class for the other direction. Just initialize with the iv
            from the other direction...
        """
        if restart or self.restart :
            Skype_RC4_Expand_IV (self.iv, self.rc4)
            self.restart = False
        return RC4_crypt (data, self.rc4)
    # end def decrypt_tcp

    def decrypt_udp (self, ip, src = None, dst = None) :
        """ Dissect a UDP packet. Silver Needle tells us that a UDP
            packet starts with a two-byte magic number (called id)
            followed by a one-byte opcode (called func), a 4-byte CRC32
            of the decrypted contents (to verify correct reception) and
            then the encrypted contents. 
            We get the full IP packet without the Ethernet part.
            EITHER USE a packet with a correct *EXTERNAL* ip in it
            or specify the correct ip as src!
            Also be sure the dst ist the *final* destination.
        """
        assert (ord (ip [0]) & 0xf0 == 0x40) # IPv4
        assert (ord (ip [9]) == 0x11) # UDP
        ipo   = 4 * (ord (ip [0]) & 0xf)
        udpo  = ipo + 8
        skypo = udpo + 3 # start of iv, crc, encrypted data

        skid  = ip [udpo:udpo+2]

        newdst = None
        if (ord (ip [udpo+2]) & 0xf) == 0x3 :
            assert (ord (ip [udpo+3]) == 0x01)
            newdst = ip [udpo+4:udpo+8]
            skypo  = udpo + 8
        if src :
            if len (src) > 4 :
                src = ''.join (chr (int (x)) for x in src.split ('.'))
        else :
            src = ip [12:16]
        if dst :
            if len (dst) > 4 :
                dst = ''.join (chr (int (x)) for x in dst.split ('.'))
        else :
            if newdst :
                dst = newdst
            else :
                dst = ip [16:20]


        seed = skype_crc (''.join (reversed (src)))
        seed = skype_crc (''.join (reversed (dst)), seed)
        seed = skype_crc (''.join (reversed (skid)), seed)
        seed = skype_crc ('\0\0', seed)
        #print "seed: %08x" % seed

        iv   = unpack ('!L', ip [skypo:skypo+4])   [0] & 0xFFFFFFFF
        crc  = unpack ('!L', ip [skypo+4:skypo+8]) [0] & 0xFFFFFFFF
        print "expect CRC: %08x" % crc

        seed = (seed ^ iv) & 0xFFFFFFFF
        #print "seed ^ iv: %08x" % seed

        self.rc4 = RC4_Context ()
        Skype_RC4_Expand_IV (seed, self.rc4)
        #print dumphex (ip [skypo+8:])
        d = RC4_crypt (ip [skypo+8:], self.rc4)
        print dumphex (d)
        print "got    CRC: %08x\n" % (skype_crc (d) & 0xFFFFFFFF)
        return d
    # end def decrypt_udp

# end class Skype_Decryptor

if __name__ == '__main__' :
    # Get a packet from pcap and try to unpack and verify CRC
    # I'm using a pcap file containing Skype traffic from
    # wireshark.org:
    # http://wiki.wireshark.org/SampleCaptures?action=AttachFile&do=get&target=SkypeIRC.cap

    import pcap

    count = 0
    decryptor = None

    def get_tcp_payload (data) :
        """ Extract payload from a tcp packet """
        assert (data [12:14] == '\x08\x00') # IP
        assert (ord (data [14]) & 0xf0 == 0x40) # IPv4
        assert (data [23] == '\x06') # TCP
        hl  = 4 * (ord (data [14]) & 0xf) + 14
        tcp = data [hl:]
        thl = (ord (tcp [12]) & 0xf0) >> 2
        return tcp [thl:]
    # end def get_tcp_payload

    def cb (pktlen, data, timestamp) :
        """ callback for packet dump
        """
        global count
        global decryptor
        count += 1
        # FIXME: We really want to follow the tcp stream in both
        # directions here and decrypt that. Not done for now.
        if count == 273 :
            sky = get_tcp_payload (data)
            decryptor = Skype_Decryptor (sky)
            # display that the first bytes are really as expected from
            # Silver Needle in the Skype:
            # After this we need to restart, this won't be necessary if
            # we didn't decrypt the first 14 bytes again.
            print dumphex (decryptor.decrypt_tcp (sky [4:14], True))
            decryptor.restart = True

            # Got more to decrypt?
            if len (sky) > 14 :
                d = decryptor.decrypt_tcp (sky [14:])
        elif count == 274 and 0 :
            sky = get_tcp_payload (data)
            d = decryptor.decrypt_tcp (sky)
            print dumphex (d)
            d = decryptor.decrypt_tcp (sky, restart = True)
            print dumphex (d)
        elif count == 216 and 0 :
            # 0x03 skype forwarding packet, not yet working
            assert (data [12:14] == '\x08\x00') # IP
            sky = data [14:]
            decryptor = Skype_Decryptor ()
            print "------------"
            d = decryptor.decrypt_udp (sky, '\x56\x80\xF5\x73', '\x24\x49\x9e\xb8')
            d = decryptor.decrypt_udp (sky, '\x56\x80\xF5\x73')
            d = decryptor.decrypt_udp (sky, '\x56\x80\xF5\x73', '\xa5\x7c\xfd\xf1')
            d = decryptor.decrypt_udp (sky, '\x24\x49\x9e\xb8', '\xa5\x7c\xfd\xf1')
            d = decryptor.decrypt_udp (sky, '\xa5\x7c\xfd\xf1', '\x24\x49\x9e\xb8')
            print "------------"
        elif count == 220 :
            assert (data [12:14] == '\x08\x00') # IP
            sky = data [14:]
            decryptor = Skype_Decryptor ()
            d = decryptor.decrypt_udp (sky, '\x56\x80\xF5\x73')
        elif count == 419 :
            assert (data [12:14] == '\x08\x00') # IP
            sky = data [14:]
            decryptor = Skype_Decryptor ()
            # other direction, set dst to the external IP
            d = decryptor.decrypt_udp (sky, dst = '\x56\x80\xF5\x73')
    # end def cb

    # Silver Needle example p. 41 is obviously a packet *before* Skype
    # knows its external IP address, so the external IP used is 0.0.0.0
    # the 172.16 address is RFC1918
    # This is the same as Vanilla Skype part 1 p. 49
    sky = \
        ('\x45\x00\x00\x2e\x00\x04\x40\x00\x40\x11\xeb\x75'
         '\xac\x10\x48\x83\x18\x62\x42\x50\x08\x03\x20\x53'
         '\x00\x1a\x21\x9c\x7f\x4e\x02\x11\x8a\xc0\x37\xfc'
         '\x95\x75\x5e\x5e\xb9\x81\x7a\x8e\xfa\x81'
        )
    decryptor = Skype_Decryptor ()
    d = decryptor.decrypt_udp (sky, '\0\0\0\0')

    # Another initial udp packet from Vanilla or Silver Needle (where
    # did I get this one from) before skype knows its external IP
    sky = \
        ('\x45\x00\x00\x2e\x00\x04\x40\x00\x40\x11\xeb\x75'
         '\xac\x10\x48\x83\xd4\x46\xcc\xd1\x08\x03\x20\x53'
         '\x00\x1a\x21\x9c\x7f\x46\x02\x09\x37\x63\xfb\xf2'
         '\x86\x24\xe6\x9a\x83)\x08K\xc6\xa8'
        )
    decryptor = Skype_Decryptor ()
    d = decryptor.decrypt_udp (sky, '\0\0\0\0')

    p = pcap.pcapObject ()
    p.open_offline ('SkypeIRC.cap')
    p.loop (419, cb)

