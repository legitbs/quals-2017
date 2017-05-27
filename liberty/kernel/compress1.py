import sys, os

data = open(sys.argv[1], "r").read()

#do histogram on the data
CharCount = [0]*256

for i in data:
	CharCount[ord(i)] += 1

#find an entry to use
UsefulEntry = [0, len(data)]
for i in xrange(0, len(CharCount)):
	if CharCount[i] < UsefulEntry[1]:
		UsefulEntry = [i, CharCount[i]]

print "Using char %02x, found %d times" % (UsefulEntry[0], UsefulEntry[1])
CompressKey = chr(UsefulEntry[0])

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
			while((i + MatchCount < len(data)) and (MatchCount < 259) and (data[i] == data[i+MatchCount])):
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

#delete the original if it exists
try:
	os.unlink(sys.argv[i] + ".a")
except:
	pass
open(sys.argv[1] + ".a", "w").write(OutData)
print "%d -> %d, %0.2f%% compressed" % (len(data), len(OutData), (1.0 - (float(len(OutData)) / float(len(data)))) * 100.0)
