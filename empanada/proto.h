#ifndef PROTO_H_
#define PROTO_H_

#include <sstream>
#include <iomanip>

#define MESSAGE_LIMIT 128

extern EMPMA g_empMem; 

typedef struct PROTOMSG {
	uint8_t type;
	uint8_t idx;
	uint8_t size;
	char data[31];
	uint32_t (*HashFunc)(const char*, uint8_t);
	struct PROTOMSG *prev;
	struct PROTOMSG *next;
} pMessage;

pMessage *g_empCurm;
uint16_t g_msgCnt;

vector<pMessage*> g_empMlist;
vector<pMessage*>::iterator si;

uint8_t empGetMtype(uint8_t);
uint8_t empGetMsize(uint8_t);
uint8_t empGetMidx(uint8_t);
uint32_t empSrvHsum(const char*, uint8_t);
uint32_t empCliHsum(const char*, uint8_t);
int empDcdPmsg(pMessage*, uint8_t);
void empDcdPmsgCmd(void);
void empSndLmsg(char*);
void empSndmsg(char *);
void empSndmsg(const char*);
const char* empGethsum(uint8_t);
const char* empGetmsg(uint8_t);
bool empRmmsg(uint8_t);
uint16_t empClrInvdmsg(void);
char* empGetall(void);
void empRmall(void);

uint8_t empGetMtype(uint8_t byte) {
	
	if (!(byte & 0b10000000)) {
		return 0;
	}

	return 1;
}

uint8_t empGetMsize(uint8_t byte) {

	return (byte & 0b00011111);
}

uint8_t empGetMidx(uint8_t byte) {

	return (byte & 0b01100000) >> 5;
}

uint32_t empSrvHsum(const char *buffer, uint8_t size) {

     uint32_t s1 = 1;
     uint32_t s2 = 0;

     for (size_t n = 0; n < size; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
     }

     return (s2 ^ s2)-1;
}

uint32_t empCliHsum(const char *buffer, uint8_t size) {

     uint32_t s1 = 1;
     uint32_t s2 = 0;

     for (size_t n = size; n > size; n--) {
        s2 = (s1 + buffer[n]) % 31337;
        s1 = (s2 - s1) % 31337;
     }     

     return ( ((s2 << 16) | s1) ^ (uint32_t)buffer );
}



int empDcdPmsg(pMessage *prev, uint8_t header, uint8_t num) {

	uint8_t buf[256];
	uint8_t type = empGetMtype(header);
	uint8_t size = empGetMsize(header);
	uint8_t index = empGetMidx(header);
	uint8_t nextHeader;

	uint8_t count = 0;

	memset(buf, 0, sizeof(buf));

	if (DEBUG)
		cout << "++ reading size: " << dec << (int)size << "\n";

	while (count < size && count < 31) {
		fread((uint8_t*)(buf + count), 1, 1, stdin);
		count++;
	}

	pMessage *msg = (pMessage*)g_empMem.alloc(sizeof(pMessage));

	if (msg == NULL) {
		cout << "[MemError] => Terminating";
		exit(-1);
	}

	memset(msg, 0, sizeof(pMessage));

	if (num == 0) {
		g_empCurm = msg;
	}

	if (prev != NULL) {
		msg->prev = prev;
		prev->next = msg;
		if (DEBUG)
			cout << "prev: " << hex << prev << "\tcurrent: " << msg << "\n";
	}
	if (type) {
		msg->HashFunc = &empCliHsum;
	} else {
		msg->HashFunc = &empSrvHsum;
	}
	
	msg->type = type;
	msg->size = size;
	msg->idx = index;
	memcpy(msg->data, buf, size);

	if (DEBUG)
		cout << "data: " << msg->data << "\n";

	num++;
	g_msgCnt++;

	if (index != 0 && num < 3) {
		nextHeader = (uint8_t)getc(stdin);
		empDcdPmsg(msg, nextHeader, num);
	}

	return 0;
}

char* empGetall(void) {
	size_t total = 0;

	for (si = g_empMlist.begin(); si < g_empMlist.end(); si++) {
			pMessage *pTmp = (pMessage*)*si;
			while (pTmp != NULL) {
				total += pTmp->size + 1;
				pTmp = pTmp->next;
			}
		
	}

	char *all = (char*)g_empMem.alloc(total+1);

	if (all == NULL) {
		cout << "[MemError] => Terminating";
		exit(-1);
	}

	for (si = g_empMlist.begin(); si != g_empMlist.end(); si++) {
			pMessage *pTmp = (pMessage*)*si;
			while (pTmp != NULL) {
				if(pTmp->idx == 0) {
					strncat(all, pTmp->data+1, pTmp->size-1);
				} else {
					strncat(all, pTmp->data, pTmp->size);
				}
				pTmp = pTmp->next;
			}
	}

	return all;
}

void empRmall(void) {
	for (si = g_empMlist.begin(); si < g_empMlist.end(); si++) {
			pMessage *pTmp = (pMessage*)*si;
			g_empMem.free(pTmp);
			memset(pTmp, 0, sizeof(pMessage));
	}
	g_empMlist.clear();
}

void empDcdPmsgCmd(void) {
	char t[128];
	uint16_t retval = 0;
	pMessage *p = g_empCurm;
	uint8_t cmd = 0;

	char msg[128];

	memset( t, 0, sizeof( t ) );
	memset( msg, 0, sizeof( msg ) );

	if (p->idx == 0 || p->idx == 1) 
	{
		cmd = (uint8_t)p->data[0];

		if (DEBUG)
			cout << "PROCESSING COMMAND: 0x" << hex << (int)cmd << dec << "\n";

		switch(cmd) {
			case 0x00:
			{
				empRmall();
				break;
			}
			case 0x10:
			{
				if (g_empMlist.size() == MESSAGE_LIMIT) {
					empSndmsg("[StoreMessage] => Return: Failed, Store Full");
					break;
				}

				g_empMlist.push_back(p);
				empSndmsg("[StoreMessage] => Return: Success");
				break;
			}
			case 0x20:
			{
				empSndmsg(empGethsum((uint8_t)p->data[1]));
				break;
			}
			case 0x30:
			{
				empSndmsg(empGetmsg((uint8_t)p->data[1]));
				break;
			}
			case 0x40:
			{
				sprintf(t, "[MsgCount] => Return: %d", g_empMlist.size());
				empSndmsg(t);
				break;
			}
			case 0x50:
			{
				retval = empRmmsg((uint8_t)p->data[1]);
				if (retval)
					empSndmsg("[DeleteMessage] => Return: Success");
				else
					empSndmsg("[DeleteMessage] => Return: Fail");
				break;
			}
			case 0x60:
			{
				empSndLmsg(empGetall());
				break;
			}
			case 0xFE:
			{
				retval = empClrInvdmsg();
				if (retval) {
					sprintf(t, "[ClearInvalid] => Return: %d", retval);
					empSndmsg(t);
				}
				else {
					empSndmsg("[ClearInvalid] => Return: Fail");
				}
				break;
			}
			default:
			{
				empSndmsg("[Terminate] => Return: Success");
				exit(0);
			}
		}

		if (cmd != 0x10) {
			pMessage *tmp;
			while (p != NULL) {
				tmp = p;
				p = p->next;
				g_empMem.free(tmp);
			}
		}
	}

	return;
}

void empSndLmsg(char *data) {
	size_t len = strlen(data);

	char header = '\x00';

	cout << header << data;
}

void empSndmsg(const char *data) {
	empSndmsg((char*)data);
}

void empSndmsg(char *data) {
	size_t len = strlen(data);

	if (len > 93) { len = 93; }

	size_t msgCount = (len / 31) + 1;
	char tmp[64];
	char header;

	for (int i = 0; i < msgCount; i++) {
		header = 0;
		memset(tmp, 0, sizeof(tmp));
		header += (msgCount << 5);

		if (msgCount == 1) {
			header += len;
			cout << header << data;
		}
		else if (msgCount == 2) {
			if (i == 0) {
				header += 31;
				memcpy(tmp, data, 31);
				cout << header << tmp;
			} else {
				header += len - 31;
				memcpy(tmp, data+32, len-31);
				cout << header << tmp;
			}
		} else {
			if (i == 0) {
				header += 31;
				memcpy(tmp, data, 31);
				cout << header << tmp;
			} else if (i == 1) {
				header += 31;
				memcpy(tmp, data+32, 31);
				cout << header << tmp;
			} else {
				header += len - 62;
				memcpy(tmp, data+63, len-62);
				cout << header << tmp;
			}
		}
	}
	return;
}

const char* empGethsum(uint8_t idx) {
	uint32_t hash;

        if (g_empMlist.size() == 0)
                return (const char*)"[GetHash] => Return: No Messages";

	if (idx > MESSAGE_LIMIT || idx > g_empMlist.size()-1)
		return (const char*)"[GetHash] => Return: Out of Range";

	pMessage *p = (pMessage*)g_empMlist[idx];

	stringstream ss;

	while (p != NULL) {
		hash = (*p->HashFunc)(p->data,p->size);

		if(DEBUG)
			cout << "HASH: 0x" << hex << hash << dec << "\n";

		ss << setfill('0') << setw(sizeof(uint32_t)*2) << hash;
		p = p->next;
	}

	return ss.str().c_str();
}

const char* empGetmsg(uint8_t idx) {
	char *buf;
	size_t len = 0;

	if (g_empMlist.size() == 0)
                return (const char*)"[GetMessage] => Return: No Messages";

	if (idx > MESSAGE_LIMIT || idx > g_empMlist.size()-1)
		return (const char*)"[GetMessage] => Return: Out of Range";
	
	pMessage *p = g_empMlist[idx];

	stringstream ss;

	while (p != NULL) {
		len += p->size;
		if (p->idx == 0)
			ss << p->data+1;
		else
			ss << p->data;
		p = p->next;
	}

	buf = (char*)g_empMem.alloc(len+1);

	memcpy(buf, ss.str().c_str(), len+1);

	return (const char*)buf;
}

bool empRmmsg(uint8_t idx) {

	if (idx > MESSAGE_LIMIT || idx > g_empMlist.size())
		return false;

	pMessage *p = g_empMlist[idx];
	pMessage *tmp;

	while (p != NULL) {
		tmp = p;
		p = p->next;
		g_empMem.free(p);
	}

	g_empMlist.erase(g_empMlist.begin()+idx);

	return true;
}

uint16_t empClrInvdmsg(void) {
	uint16_t count = 0;

	for (si = g_empMlist.begin(); si < g_empMlist.end(); si++) {
		pMessage *p = *si;
		pMessage *tmp;

		while (p != NULL) {
			tmp = p;
			p = p->next;
			if ((*tmp->HashFunc)(tmp->data,tmp->size) == 0xffffffff) {
				g_empMem.free(tmp);
				count++;
			}
		}
	}

	return count;
}

#endif
