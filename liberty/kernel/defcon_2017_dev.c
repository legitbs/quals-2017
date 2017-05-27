#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include "defcon_2017_dev.h"

//define a couple structures for the 4.10 kernel layout, this is to allow compiling on non 4.10 kernels
//when building the challenge
struct file_operations_defcon {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
        ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
        int (*iterate) (struct file *, struct dir_context *);
        int (*iterate_shared) (struct file *, struct dir_context *);
        unsigned int (*poll) (struct file *, struct poll_table_struct *);
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);
        int (*fsync) (struct file *, loff_t, loff_t, int datasync);
        int (*fasync) (int, struct file *, int);
        int (*lock) (struct file *, int, struct file_lock *);
        ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        int (*check_flags)(int);
        int (*flock) (struct file *, int, struct file_lock *);
        ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
        ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
        int (*setlease)(struct file *, long, struct file_lock **, void **);
        long (*fallocate)(struct file *file, int mode, loff_t offset,
                          loff_t len);
        void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
        unsigned (*mmap_capabilities)(struct file *);
#endif
        ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
                        loff_t, size_t, unsigned int);
        int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
                        u64);
        ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
                        u64);
};

struct miscdevice_defcon  {
        int minor;
        const char *name;
        const struct file_operations_defcon *fops;
        struct list_head list;
        struct device *parent;
        struct device *this_device;
        const struct attribute_group **groups;
        const char *nodename;
        umode_t mode;
};

long defcon_2017_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	//the original string is "DEFCON CTF \x20\x17 Lightning Was Here"
	//we xor it with the xorkey then protected the key with another xor
	//the key xor is flipped to avoid the compiler from just undoing an xor mask
	//the key is "cLEMENCy" as a fun nod to the architecture name
	unsigned long KnockVals[4] = {0x2709030e0a00633a, 0x370a656d526e0f10, 0x042431232c202459, 0x342d366d0d2b311c};
	unsigned long KeyXor = 0xfc38b42ec3023f1c;
	unsigned long KnockXor = 0x657c4c8663f1749f;
	int ShiftAmount;
	unsigned long ReqKnockVal;

	//if too many knocks then fail and tell them
	if((InitialWrite != 2) || (KnockCount >= REQ_KNOCKS))
	{
		memset(FlagData, 0, sizeof(FlagData));
		memset(DecIV, 0, sizeof(DecIV));
		memset(DecKey, 0, sizeof(DecKey));
		return -EINVAL;
	}

	//figure out the value
	ReqKnockVal = KnockVals[KnockCount / 8] ^ KeyXor;

	//top byte 1st
	ShiftAmount = 56 - ((KnockCount % 8) * 8);
	ReqKnockVal = (ReqKnockVal >> ShiftAmount) & 0xff;

	//find the right knockxor to undo the key xor
	ShiftAmount = ((KnockCount % 8) * 8);
	ReqKnockVal ^= ((KnockXor >> ShiftAmount) & 0xff);
	DecKey[KnockCount] ^= ReqKnockVal;

	//wipe things out if they fail but don't tell them
	if(cmd != _IO('z', ReqKnockVal))
	{
		memset(FlagData, 0, sizeof(FlagData));
		memset(DecIV, 0, sizeof(DecIV));
		memset(DecKey, 0, sizeof(DecKey));
		return 0;
	}

	//increase the knock count
	KnockCount++;
	return 0;
}

static const struct file_operations_defcon defcon_2017_fops = {
	.owner		= THIS_MODULE,
	.read		= defcon_2017_read,
	.write		= defcon_2017_write,
	.unlocked_ioctl = defcon_2017_ioctl,
};

static struct miscdevice_defcon defcon_2017_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "defcon_2017",
	.fops = &defcon_2017_fops,
        .mode = S_IRUSR | S_IWUSR,
};

static int __init
defcon_2017_init(void)
{
	int ret;

	//register the device
	ret = misc_register(&defcon_2017_dev);
	if (ret)
		printk(KERN_ERR "Unable to register defcon 2017 misc device\n");

	//make sure entries are 0
	InitialRead = 0;
	InitialWrite = 0;
	KnockCount = 0;
	memset(DecKey, 0, sizeof(DecKey));
	memset(DecIV, 0, sizeof(DecIV));

	return ret;
}

module_init(defcon_2017_init);

static void __exit
defcon_2017_exit(void)
{
	misc_deregister(&defcon_2017_dev);
}

module_exit(defcon_2017_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lightning");
MODULE_DESCRIPTION("defcon 2017 module");
MODULE_VERSION("quals");
