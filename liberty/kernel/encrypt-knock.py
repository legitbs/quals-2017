import sys, os

Key = "cLEMENCy"
KnockData = "DEFCON CTF \x20\x17 Lightning Was Here"

KnockVals = [0,0,0,0]
KeyVal = 0

for i in xrange(0, len(KnockData)):
	KnockVals[i / 8] <<= 8
	KnockVals[i / 8] |= ord(KnockData[i])

for i in xrange(0, len(Key)):
	KeyVal <<= 8
	KeyVal |= ord(Key[i])

for i in xrange(0, len(KnockVals)):
	KnockVals[i] ^= KeyVal

KeyXorVal = 0x657c4c8663f1749f
for i in xrange(0, len(Key)):
	XorShiftAmount = i*8
	KeyShiftAmount = 56-XorShiftAmount
	print "xor: %016x" % (((KeyXorVal >> XorShiftAmount) & 0xff) << KeyShiftAmount)
	KeyVal ^= (((KeyXorVal >> XorShiftAmount) & 0xff) << KeyShiftAmount)

print "unsigned long KnockVals[4] = {0x%016x, 0x%016x, 0x%016x, 0x%016x};" % (KnockVals[0], KnockVals[1], KnockVals[2], KnockVals[3])
print "unsigned long KeyXor = 0x%016x;" % (KeyVal)
print "unsigned long KnockXor = 0x%016x;" % (KeyXorVal)

