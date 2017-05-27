import sys, os
import binascii

data = open(sys.argv[1], "r").read()
origdata = data

#add our starting header, 0 indicates count of 0
data = chr(0) + data

#start compressing, we are looking for duplicate data, 100 to 3 bytes
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
		if CharCount[i] < UsefulEntry[1]:
			UsefulEntry = [i, CharCount[i]]

	#print "Using char %02x, found %d times" % (UsefulEntry[0], UsefulEntry[1])
	CompressKey = chr(UsefulEntry[0])

	DataList = []
	CurData = CompressKey

	#make a copy as we will be modifying it
	data = SmallestData

	#replace all entries of the compress key with key + chr(255)
	data = data.replace(CompressKey, CompressKey + chr(255))

	for i in xrange(255, 2, -1):
		#if too many entries then stop
		if len(DataList) == 255:
			break

		pos = 0
		while(pos < len(data)):
			#if too many entries then stop
			if len(DataList) == 255:
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

	print "Key: %02x, %d entries" % (ord(CompressKey), len(DataList))

	#add the modified version
	CurData += data

	SmallestData = CurData

#delete the original if it exists
try:
	os.unlink(sys.argv[i] + ".a")
except:
	pass
open(sys.argv[1][0:-1] + "b", "w").write(SmallestData)
print "%d -> %d, %0.2f%% compressed" % (len(origdata), len(SmallestData), (1.0 - (float(len(SmallestData)) / float(len(origdata)))) * 100.0)
