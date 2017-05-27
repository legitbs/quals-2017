import sys, os
import struct

if len(sys.argv) != 3:
	print "Usage: %s lib FixedOffset" % (sys.argv[0])
	sys.exit(0)

data = open(sys.argv[1],"r").read()
NewOffset = int(sys.argv[2], 16)

#NewOffset = 0x7ff879bc0000
#NewOffset = 0

print "Moving %s to 0x%016lx" % (sys.argv[1], NewOffset)

#make sure we are 64bit
if data[4] != '\x02':
	print "Not a 64-bit library"
	sys.exit(0)

HeaderOffset = struct.unpack("<Q", data[0x20:0x28])[0]
HeaderCount = struct.unpack("<H", data[0x38:0x3A])[0]

print "Header at 0x%x, %d entries" % (HeaderOffset, HeaderCount)

Found = False
for i in xrange(0, HeaderCount):
	Offset = HeaderOffset+(i*0x38)+0x18
	if struct.unpack("<I", data[Offset-0x18:Offset-0x14])[0] == 0x6474e552:
		print "Found GNU_RELRO, modifying"
		CurVal = struct.unpack("<Q", data[Offset-8:Offset])[0]
		#CurVal &= 0xffffffff
		CurVal = NewOffset | (CurVal & 0xfff);
		data = data[0:Offset] + struct.pack("<Q", CurVal) + data[Offset+8:]
		Found = True
		break

if not Found:
	print "Failed to locate GNU_RELRO header"
	sys.exit(0)

open(sys.argv[1],"w").write(data)
