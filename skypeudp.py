
#
# UDP capture analysis
#

possible_message_types = ...

# PCAP: open capture

# parse UDP packets

if packet type is UDP.
	msg_type = data[2]
	if msg_type in possible_message_types:
		if msg_type == 0x02 # payload
			parse_payload(data[3:])

# example output:
# 172.16.72.131:2051 > 212.70.204.209:23410 / Skype SoF id=0x7f46 func=0x2 / Skype Enc / Skype Cmd cmd=27L r

