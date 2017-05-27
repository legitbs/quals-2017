#include <json/json.h>
#include <json/reader.h>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <zlib.h>
#include <mysql/mysql.h>
#include <signal.h>

#include "lib/OpenSSL.h"
#include "lib/ZLib.h"
#include "lib/Base64.h"

unsigned char *SBox;
#define SBOX_SIZE 65536

typedef void (*sighandler_t)(int);
#define AlarmTime 30

static void alarmHandle(int sig)
{
	write(1, "Alarmed\n", 8);
	_exit(0);
}

int main(int argc, char **argv)
{
	unsigned int i;
	unsigned short j;
	short z;
	unsigned char *Data;
	unsigned int DataLen;
	int KeyLen;
	char LogName[256];
	std::ifstream inFile("config");
	Json::Value root;
	Json::Value CurJSON;
	Json::Reader JReader;
	OpenSSL *SSL;
	unsigned char *JSONData;
	std::string JSONString;
	long JSONDataSize;
	ZLib *ZLibData;
	Base64 B64;
	Json::Value JSONSBox;
	std::ofstream outFile;
	std::string SQL;

	//make sure we stop if they try to just hang the connection open
        signal(SIGALRM, (sighandler_t)alarmHandle);
        alarm(AlarmTime);

	SBox = (unsigned char *)malloc(SBOX_SIZE);
/*
	std::ifstream *MapFile;
	std::ofstream *f = new std::ofstream("log.txt", std::ios::trunc);
	MapFile = new std::ifstream("/proc/self/maps");
	if(MapFile->fail())
	{
		std::cout << "Failed to open map file\n";
		return 0;
	}

	while(!MapFile->eof())
	{
		MapFile->getline(LogName, sizeof(LogName));
		*f << LogName << "\n";
	}
	f->close();
	delete(MapFile);
	delete(f);
*/
	//make sure we could open the config
	if(inFile.fail())
	{
		std::cout << "Failed to open config\n";
		return 0;
	}

	//get the line for the log file name
	inFile.getline(LogName, sizeof(LogName));
	inFile.close();

 	SSL = new OpenSSL((char *)"cert.pem", (char *)"key.pem");
	SSL->Connect(0, 1);

	//SSL->WriteString("Connected\n");

	//go get all of the data
	ZLibData = new ZLib(SSL, 1024*1024, 1024*1024);
	if(!ZLibData->ReadHeader())
	{
		SSL->WriteString("Unable to initialize ZLib\n");
		SSL->Disconnect();
		_exit(0);
	}

	JSONDataSize = ZLibData->DecompressData(&JSONData);
	if(JSONDataSize <= 0)
	{
		SSL->WriteString("Unable to decompress\n");
		SSL->Disconnect();
		_exit(0);
	}

	//create the json object
	JSONString.assign((char *)JSONData, JSONDataSize);
	if(!JReader.parse(JSONString, root, false))
	{
		SSL->WriteString("Failed to parse json\n");
		SSL->Disconnect();
		_exit(0);
	}

	//if no data then fail
	if(!root.isMember("Data") || !root["Data"].isString())
	{
		//can't find data, fail
		SSL->WriteString("Failed to locate Data\n");
		SSL->Disconnect();
		_exit(0);
	}

	//base 64 decode the data
	//B64 = new(Base64);
	DataLen = B64.decode(root["Data"].asString(), (char **)&Data);
	if(!DataLen)
	{
		SSL->WriteString("Invalid base64 data\n");
		SSL->Disconnect();
		_exit(0);
	}

	//make sure we have key, that key is an array, and all array entries are numbers
	if(!root.isMember("Key") || !root["Key"].isArray())
	{
		//can't find Key, fail
		SSL->WriteString("Failed to locate Key\n");
		SSL->Disconnect();
		_exit(0);
	}

	//make sure it is long enough and odd just to annoy people
	KeyLen = root["Key"].size();
	if((KeyLen < 32) || !(KeyLen & 1))
	{
		SSL->WriteString("Invalid key length\n");
		SSL->Disconnect();
		_exit(0);
	}

	//make sure it is a valid key, all integers
	for(i = 0; i < KeyLen; i++)
	{
		CurJSON = root["Key"][i];
		if(!CurJSON.isInt())
		{
			SSL->WriteString("Invalid key value\n");
			SSL->Disconnect();
			_exit(0);
		}
	}

	//start our SBox setup
	for(i = 0; i < SBOX_SIZE; i++)
		SBox[i] = i;

	//if we have array entries then put them in place
	if(root.isMember("SBox") && root["SBox"].isArray())
	{
		/*
		//make sure enough entries exist
		if(root["SBox"].size() % 2)
		{
			SSL->WriteString("Invalid SBox array\n");
			SSL->Disconnect();
			_exit(0);
		}
		*/
		//validate all numeric
		JSONSBox = root["SBox"];
		for(i = 0; i < JSONSBox.size(); i++)
		{
			if(!JSONSBox[i].isInt())
			{
				SSL->WriteString("Invalid SBox value\n");
				SSL->Disconnect();
				_exit(0);
			}
		}

		//get all entries, BUG, z is signed allowing negative indexing
		for(i = 0; i < JSONSBox.size(); i+=2)
		{
			z = JSONSBox[i].asInt();
			j = JSONSBox[i+1].asInt();
			SBox[z] = j;
		}
	}

	//setup the key into the sbox
	for(i = 0, j = 0; i < SBOX_SIZE; i++)
	{
		//swap entries
		CurJSON = root["Key"][i % KeyLen];
		j += SBox[i] + CurJSON.asInt();

		SBox[i] = SBox[i] ^ SBox[j];
		SBox[j] = SBox[i] ^ SBox[j];
		SBox[i] = SBox[i] ^ SBox[j];
	}

	//decrypt
	for (i = 0, j = 0; i < DataLen; i++)
	{
		j += *(unsigned short *)&SBox[i];

		//swap entries
		SBox[i] = SBox[i] ^ SBox[j];
		SBox[j] = SBox[i] ^ SBox[j];
		SBox[i] = SBox[i] ^ SBox[j];

		//decrypt
		Data[i] = Data[i] ^ SBox[*(unsigned short *)&SBox[i] + *(unsigned short *)&SBox[j]];
	}

	//Data now contains the decrypted version
	//see if it contains "D'ni" at the beginning
	if(*(unsigned int *)&Data[0] == 0x696e2744)
	{
		//go do random checks that are useless, the file is empty
		//open our log and write it
		if(memcmp(LogName, "TXT ", 4) == 0)
		{
			outFile.open(&LogName[4], std::ios::app);
			if(!outFile.fail())
			{
				outFile.write((char *)&Data[4], DataLen - 4);
				outFile.close();
			}
		}
		else if(memcmp(LogName, "SQL3 ", 5) == 0)
		{
			//split up the entries
			sqlite3 *db;
			char *err_msg = 0;

			if(sqlite3_open(&LogName[5], &db) == SQLITE_OK)
			{
				SQL = "Insert into Log('";
				SQL += (char *)&Data[4];
				SQL += "');";
				sqlite3_exec(db, SQL.c_str(), 0, 0, &err_msg);
				sqlite3_close(db);
			}
		}
		else if(memcmp(LogName, "CONN ", 5) == 0)
		{
			//write back to ourselves
			SSL->Write((char *)&Data[4], DataLen - 4);
		}
		else if(memcmp(LogName, "SYSLOG", 6) == 0)
		{
			openlog("nekisahloth", LOG_PID, 0);
			syslog(LOG_INFO, (char *)&Data[4], DataLen - 4);
			closelog();
		}
		else if(memcmp(LogName, "MYSQL ", 6) == 0)
		{
			MYSQL *con;
			char *Host;
			char *User;
			char *Pass;
			char *DB;

			//go figure out the host, user, pass, and database, all space delimited
			Host = &LogName[7];

			//get user
			User = strchr(Host, ' ');
			*User = 0;
			User++;

			//get password
			Pass = strchr(User, ' ');
			*Pass = 0;
			Pass++;


			//get db
			DB = strchr(Pass, ' ');
			*DB = 0;
			DB++;

			con = mysql_init(NULL);
			mysql_real_connect(con, Host, User, Pass, DB, 0, NULL, 0);
			SQL = "Insert into Log('";
			SQL += (char *)&Data[4];
			SQL += "');";
			mysql_query(con, SQL.c_str());
			mysql_close(con);
		}
	}

	//disconnect but fail to delete the object, etc
	SSL->Disconnect();
	_exit(0);
	return 0;
}
