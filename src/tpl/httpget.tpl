#
# 
#

# HTTP Get Request Template for the Flood Network Packet Generator

Z1:String=GET @url@@SPACE@
Z2:String=HTTP/1.0@CRLF@
Z3:String=Host: @host@@CRLF@
Z4:String=User-Agent: Flood v0.2.3 - Your friendly network packet generator@CRLF@
Z5:String=Accept: */*@CRLF@
Z6:ByteField=@CRLF@
