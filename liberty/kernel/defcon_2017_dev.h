#ifndef DEFCON_CTF_2017
#define DEFCON_CTF_2017

#define REQ_KNOCKS 32
#include <linux/miscdevice.h>

extern char DecKey[32];
extern char DecIV[16];
extern char FlagData[128];
extern int InitialRead;
extern int KnockCount;
extern int InitialWrite;

typedef struct _sAesData
{
	char	*in_block;
	char	*out_block;
	char	*expanded_key;		
	char	*iv;					// for CBC mode
	long	num_blocks;
} sAesData;

int GetReadSize(void);
void iDecExpandKey256(char *, char *);
void iDec256_CBC(sAesData *);

ssize_t defcon_2017_write(struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t defcon_2017_read(struct file * file, char * buf, size_t count, loff_t *ppos);

#endif
