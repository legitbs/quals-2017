import sys, os
import struct

def FindBestChar(data):
	#do histogram on the data
	CharCount = [0]*256

	for i in data:
		CharCount[ord(i)] += 1

	#find an entry to use
	UsefulEntry = [0, len(data)]
	for i in xrange(0, len(CharCount)):
		if CharCount[i] < UsefulEntry[1]:
			UsefulEntry = [i, CharCount[i]]

	return UsefulEntry

data = open(sys.argv[1], "r").read()
OrigData = data

#########################
#layer 1 compression
#########################
print "Layer 1"
Layer1Size = len(data)

#print "Using char %02x, found %d times" % (UsefulEntry[0], UsefulEntry[1])
CompressKey = chr(FindBestChar(data)[0])

#now start looking for all the data that duplicates, if found then replace it with our special byte
#need at least 4 to be useful otherwise ignore it as the format is the following
#Key Count DupeByte
#if count is 0 then just output the key byte itself as it is supposed to exist

#first byte is our key to decompress with
OutData = CompressKey
i = 0
while(i < len(data)):
	if i < (len(data) - 3):
		#if 4 match then count how many to do
		if (data[i] == data[i+1]) and (data[i] == data[i+2]) and (data[i] == data[i+3]):
			MatchCount = 4
			while((i + MatchCount < len(data)) and (MatchCount < 258) and (data[i] == data[i+MatchCount])):
				MatchCount += 1

			#output our special flag, length, and char. length is 1 to 255 translating to 4 to 259
			OutData += CompressKey + chr(MatchCount - 3) + data[i]
			i += (MatchCount - 1)	#due to +1 in the loop
		elif data[i] == CompressKey:
			#found our compress key, handle it
			OutData += CompressKey + '\x00'
		else:
			OutData += data[i]

	elif data[i] == CompressKey:
		#found our compress key, handle it
		OutData += CompressKey + '\x00'
	else:
		OutData += data[i]
	i += 1

print "Layer1: %d -> %d" % (Layer1Size, len(OutData))

#########################
#layer 2 compression
#########################
print "Layer 2"
Layer2Size = len(OutData)

#add our starting header, 0 indicates count of 0
data = chr(0) + OutData

#we did try running the following code in a loop but the end compression was worse

CurData = ""

UsefulEntry = FindBestChar(data);
print "Using char %02x, found %d times" % (UsefulEntry[0], UsefulEntry[1])
CompressKey = chr(FindBestChar(data)[0])

DataList = []
CurData = CompressKey

#replace all entries of the compress key with key + chr(255)
data = data.replace(CompressKey, CompressKey + chr(255))

print "Data len", len(data)

for i in xrange(255, 2, -1):
	#if too many entries then stop
	if len(DataList) == 256:
		break

	pos = 0
	while(pos < len(data)):
		#if too many entries then stop
		if len(DataList) == 256:
			break

		#if we hit the data entry that matches our search byte then insert a dummy entry to avoid messing up data
		if len(DataList) == ord(CompressKey):
			DataList += [CompressKey + CompressKey]

		#if we find an entry then find/replace it all
		if (data.find(data[pos:pos+i], pos+i) != -1) and (data[pos:pos+i] not in DataList):
			#mass find/replace
			DataList += [data[pos:pos+i]]
			data = data.replace(data[pos:pos+i], CompressKey + chr(len(DataList) - 1))

		#next position
		pos += 1

#now create the entries for curdata
for i in xrange(0, len(DataList)):
		CurData += chr(len(DataList[i])) + DataList[i]

#add the modified version
SmallestData = CurData + data

print "Layer2: %d -> %d" % (Layer2Size, len(SmallestData))

#########################
#layer 3 compression
#########################
print "Layer 3"
Layer3Size = len(SmallestData)

#get our special character
SpecialKey = FindBestChar(SmallestData)
if SpecialKey[1] != 0:
	print "Unable to compress, all byte values used"
	sys.exit(0)

SpecialKey = chr(SpecialKey[0])

#add our starting header
data = SpecialKey + SmallestData

#start compressing, we are looking for 2 byte pairs to replace
SmallestData = data + " "
CurData = data
while(len(CurData) < len(SmallestData)):
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
		Entry = (ord(SmallestData[i+1]) << 8) | ord(SmallestData[i])
		WordCount[Entry] += 1

	#find the most used
	BestMatch = [0, 0]
	for i in xrange(0, len(WordCount)):
		if WordCount[i] > BestMatch[1]:
			BestMatch = [i, WordCount[i]]

	#we need at least 3 entries to be useful. 3 bytes in header + 1 per replacement. with 2 replacements = 5 bytes while original is only 4
	if BestMatch[1] >= 3:
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
	else:
		CurData = data

#add our special key, when found again we stop this loop and it is used to indicate bytes to not replace
OutData = SpecialKey + SmallestData
print "Layer3: %d -> %d" % (Layer3Size, len(OutData))

#delete the original if it exists
try:
	os.unlink(sys.argv[i] + ".a")
except:
	pass

OutData = struct.pack("<H", len(OrigData)) + OutData
open(sys.argv[1] + ".a", "w").write(OutData)
print "%d -> %d, %0.2f%% compressed" % (len(OrigData), len(OutData), (1.0 - (float(len(OutData)) / float(len(OrigData)))) * 100.0)
