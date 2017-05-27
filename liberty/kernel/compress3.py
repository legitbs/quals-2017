import sys, os

data = open(sys.argv[1], "r").read()
origdata = data

#do histogram on the data
CharCount = [0]*256

for i in data:
	CharCount[ord(i)] += 1

#find an entry to use
UsefulEntry = [0, len(data)]
for i in xrange(0, len(CharCount)):
	if CharCount[i] < UsefulEntry[1]:
		UsefulEntry = [i, CharCount[i]]

if UsefulEntry[1] != 0:
	print "Unable to compress, all bytes used"
	sys.exit(0)

#print hex(UsefulEntry[0]), UsefulEntry[1]

#get our special character
SpecialKey = chr(UsefulEntry[0])

#add our starting header
data = SpecialKey + data

#start compressing, we are looking for 2 byte pairs to replace
SmallestData = data + " "
CurData = data

Counter = 0

while(len(CurData) < len(SmallestData)):
	Counter += 1
	if Counter == 4:
		break
	#make sure we know what our smallest is then re-generate on it
	SmallestData = CurData
	CurData = ""

	#do histogram on the data
	CharCount = [0]*256

	for i in SmallestData:
		CharCount[ord(i)] += 1

	#find an entry to use
	UsefulEntry = [0, len(SmallestData)]
	for i in xrange(0, len(CharCount)):
		#not allowed to use the special key
		if i == ord(SpecialKey):
			continue

		if CharCount[i] < UsefulEntry[1]:
			UsefulEntry = [i, CharCount[i]]

	#print "Using char %02x, found %d times" % (UsefulEntry[0], UsefulEntry[1])
	CompressKey = chr(UsefulEntry[0])

	DataList = []
	CurData = CompressKey

	#make a copy as we will be modifying it
	data = SmallestData

	#make a histogram of 2 byte combos
	WordCount = [0]*0x10000
	for i in xrange(0, len(SmallestData) - 1):
		if (SmallestData[i] == CompressKey) or (SmallestData[i+1] == CompressKey):
			continue
		Entry = (ord(SmallestData[i+1]) << 8) | ord(SmallestData[i])
		WordCount[Entry] += 1

	#find the most used
	BestMatch = [0, 0]
	for i in xrange(0, len(WordCount)):
		if WordCount[i] > BestMatch[1]:
			BestMatch = [i, WordCount[i]]

	if BestMatch[1] >= 3:
		print "Key: %02x, Match: %04x, count %d" % (ord(CompressKey), BestMatch[0], BestMatch[1])

		#create our new data
		RepChars = chr(BestMatch[0] & 0xff) + chr(BestMatch[0] >> 8)
		CurData = CompressKey + RepChars
		i = 0
		while(i < len(data)):
			if ord(data[i]) == ord(CompressKey):
				#found our compress key, leave it alone
				CurData += CompressKey + SpecialKey
			elif (i < (len(data) - 2)) and (ord(data[i]) == (BestMatch[0] & 0xff)) and (ord(data[i+1]) == ((BestMatch[0] >> 8) & 0xff)) and (SpecialKey != data[i+2]):
				#found an entry to replace
				CurData += CompressKey
				i += 1
			else:
				CurData += data[i]
			i += 1;
		
		"""		
		#replace the compress key with the special key to indicate this entry shouldn't be replaced
		data = data.replace(CompressKey, CompressKey + SpecialKey)

		#replace all entries and add our header
		CurData = CompressKey + RepChars + data.replace(RepChars, CompressKey)
		"""
	else:
		CurData = data

#add our special key, when found again we stop this loop and it is used to indicate bytes to not replace
SmallestData = SpecialKey + SmallestData

#delete the original if it exists
try:
	os.unlink(sys.argv[i] + ".a")
except:
	pass
open(sys.argv[1][0:-1] + "c", "w").write(SmallestData)
print "%d -> %d, %0.2f%% compressed" % (len(origdata), len(SmallestData), (1.0 - (float(len(SmallestData)) / float(len(origdata)))) * 100.0)
