#include "syscall64.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct linux_dirent {
               unsigned long  d_ino;     /* Inode number */
               unsigned long  d_off;     /* Offset to next linux_dirent */
               unsigned short d_reclen;  /* Length of this linux_dirent */
               char           d_name[256];  /* Filename (null-terminated) */
                                 /* length is actually (d_reclen - 2 -
                                    offsetof(struct linux_dirent, d_name)) */
           } linux_dirent;

int memcmp(const unsigned char *In1, const unsigned char *In2, int Len)
{
	while(Len)
	{
		if(*In1 != *In2)
			return 1;
		In1++;
		In2++;
		Len--;
	};

	return 0;
}

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	struct stat DirInfo;
	long SysRet;
	int Found = 0;
	int DirFD;
	struct linux_dirent DirEntry;
	struct linux_dirent *CurDirEntry;
	long DirSysRet;
	char DirFolder[4096];
	int DirNameLen;

	//we should be root, start looking for /root/.ssh
	//then get a listing of /home folders and check each

	//check /root/.ssh to exist
	SysRet = my_syscall(__NR_stat, (long)"/root/.ssh", (long)&DirInfo, 0, 0, 0, 0);
	if((SysRet < 0) && (SysRet != -2))
	{
		*(long *)Data = SysRet;
		return 8;
	}

	//if it exists then check it for an authorized key
	if(SysRet == 0)
	{
		SysRet = my_syscall(__NR_stat, (long)"/root/.ssh/authorized_keys", (long)&DirInfo, 0, 0, 0, 0);
		if((SysRet < 0) && (SysRet != -2))
		{
			*(long *)Data = SysRet;
			return 8;
		}
		else if(SysRet == 0)
			Found++;
	}

	//get the folder listing from /home and start checking them all
	DirFD = my_syscall(__NR_open, (long)"/home", O_DIRECTORY, 0, 0, 0, 0);
	if(DirFD < 0)
	{
		*(long *)Data = DirFD;
		return 8;
	}

	//get each entry
	while(1)
	{
		//get the next entry
		DirSysRet = my_syscall(__NR_getdents, DirFD, &DirEntry, sizeof(DirEntry), 0, 0, 0);
		if(DirSysRet == 0)
			break;

		//had an error
		if(DirSysRet < 0)
		{
			//close it
			my_syscall(__NR_close, DirFD, 0, 0, 0, 0, 0);
			*(long *)Data = SysRet;
			return 8;
		}

		//work across all directory entries
		CurDirEntry = &DirEntry;
		while(1)
		{
			//if no room left for an entry then fail
			if(DirSysRet < (8+8+2+1))
				break;

			//if no inode then exit
			if(CurDirEntry->d_ino == 0)
				break;

			//if . or .. then skip
			if((memcmp(CurDirEntry->d_name, ".", 2) == 0) || (memcmp(CurDirEntry->d_name, "..", 3) == 0))
			{
				DirSysRet -= CurDirEntry->d_reclen;
				CurDirEntry = (((long)CurDirEntry) + CurDirEntry->d_reclen);
				continue;
			}

			//go add the name to /home folder and pull stats for .ssh
			memcpy(DirFolder, "/home/", 6);
			DirNameLen = CurDirEntry->d_reclen-8-8-2-1;
			memcpy(&DirFolder[6], CurDirEntry->d_name, DirNameLen);
			DirNameLen += 6;

			//walk to the next entry
			DirSysRet -= CurDirEntry->d_reclen;

			CurDirEntry = (((long)CurDirEntry) + CurDirEntry->d_reclen);

			//step back before the null byte
			while(DirFolder[DirNameLen-1] == 0)
				DirNameLen--;
			memcpy(&DirFolder[DirNameLen], "/.ssh", 6);
			DirNameLen += 5;

			//pull stats
			SysRet = my_syscall(__NR_stat, (long)DirFolder, (long)&DirInfo, 0, 0, 0, 0);
			if((SysRet < 0) && (SysRet != -2))
			{
				*(long *)Data = SysRet;
				return 8;
			}
			else if(SysRet == 0)
			{
				//add on "authorized_keys" and look for it
				memcpy(&DirFolder[DirNameLen], "/authorized_keys", 17);
				SysRet = my_syscall(__NR_stat, (long)DirFolder, (long)&DirInfo, 0, 0, 0, 0);
				if((SysRet < 0) && (SysRet != -2))
				{
					*(long *)Data = SysRet;
					return 8;
				}
				else if(SysRet == 0)
					Found++;
			}
		}
	}

	//close it
	my_syscall(__NR_close, DirFD, 0, 0, 0, 0, 0);

	//all good
	if(Found)
	{
		memcpy(Data, "SSH Found", 9);
		return 9;
	}
	else
	{
		*(long *)Data = 0;
		return 8;
	}
}

