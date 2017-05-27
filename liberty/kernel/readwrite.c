#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include "defcon_2017_dev.h"

char DecKey[32];
char DecIV[16];
int InitialRead;
int InitialWrite;
int KnockCount;

ssize_t defcon_2017_write(struct file * file, const char * buf, size_t count, loff_t *ppos)
{
	//see if we are filling in the IV or key
	if((count == 16) && (InitialWrite == 0))
	{
		copy_from_user(DecIV, buf, count);
		InitialWrite++;
	}
	else if((count == 32) && (InitialWrite == 1))
	{
		copy_from_user(DecKey, buf, count);
		InitialWrite++;
	}
	else
	{
		//fail it
		memset(FlagData, 0, sizeof(FlagData));
		memset(DecIV, 0, sizeof(DecIV));
		memset(DecKey, 0, sizeof(DecKey));
		return -EINVAL;
	}

	*ppos = count;
	return count;
}

ssize_t defcon_2017_read(struct file * file, char * buf, size_t count, loff_t *ppos)
{
	sAesData aesData;
	char OutBuffer[200];
	char DecExpandKey[256];
	int Ret;

	//an intial read of the encrypted key is done to see if the device is actually running
	if(InitialRead == 0)
	{
		InitialRead = 1;
		if(count != GetReadSize())
		{
			//someone is guessing, wipe data out
			memset(FlagData, 0, sizeof(FlagData));
			memset(DecIV, 0, sizeof(DecIV));
			memset(DecKey, 0, sizeof(DecKey));
			return -EINVAL;
		}

		//copy off part of the encrypted flag to the user
		Ret = copy_to_user(buf, FlagData, count);
		*ppos = count;
		return count;
	}

	//if not enough knocks done then fail
	if((KnockCount != REQ_KNOCKS) || (InitialWrite != 2))
	{
		memset(FlagData, 0, sizeof(FlagData));
		memset(DecIV, 0, sizeof(DecIV));
		memset(DecKey, 0, sizeof(DecKey));
		return -EINVAL;
	}

	//if the read size isn't 128 bytes then fail
	if(count != 128)
	{
		memset(FlagData, 0, sizeof(FlagData));
		memset(DecIV, 0, sizeof(DecIV));
		memset(DecKey, 0, sizeof(DecKey));
		return -EINVAL;
	}

	//decrypt the data
	aesData.in_block = FlagData;
	aesData.out_block = OutBuffer;
	aesData.expanded_key = DecExpandKey;
	aesData.num_blocks = sizeof(FlagData) >> 4;
	aesData.iv = DecIV;

	//setup the key and decrypt
	iDecExpandKey256(DecKey, DecExpandKey);
	iDec256_CBC(&aesData);

	//copy the data then wipe out the original
	Ret = copy_to_user(buf, OutBuffer, count);
	memset(OutBuffer, 0, sizeof(OutBuffer));
	memset(FlagData, 0, sizeof(FlagData));
	memset(DecIV, 0, sizeof(DecIV));
	memset(DecKey, 0, sizeof(DecKey));

	//if error indicate as such
	if(Ret)
		return -EINVAL;

	//all done
	*ppos = count;
	return count;
}
