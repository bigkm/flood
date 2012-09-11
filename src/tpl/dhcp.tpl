#
# $Id: dhcp.tpl,v 1.2 2008-09-07 19:31:54 welle Exp $
#

# DHCP Discover Template for the Flood Network Packet Generator

MessageType:Byte=1
HardwareType:Byte=1
HardwareAddressLength:Byte=6
Hops:Byte=@hops@
TransActionID:ByteField=@xid@
Time:ByteField=@time@
Flags:ByteField=@flags@
CIADDR:ByteField=@ciaddr@
YIADDR:ByteField=@yiaddr@
SIADDR:ByteField=@siaddr@
GIADDR:ByteField=@giaddr@
ClientHWAddress:ByteField=@cha@
ServerHostName:ByteField=@sip@
BootFileName:ByteField=@bfn@
OptMagic:ByteField=99,130,83,99

# Now append the options
DHCPMessageType:ByteField=53,1,1

# Mark end of options
EndOption:Byte=255
