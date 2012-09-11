#
# $Id: dns.tpl,v 1.3 2008-09-07 19:31:54 welle Exp $
#

# DNS Query Template for the Flood Network Packet Generator

TransActionID:ByteField=@xid@
Flags:ByteField=@flags@
Questions:ByteField=0,1
AnswerRR:ByteField=0,0
AuthorityRR:ByteField=0,0
AdditionalRR:ByteField=0,0

# Query
Name:ByteField=@name@
Type:ByteField=@type@
Class:ByteField=@class@
