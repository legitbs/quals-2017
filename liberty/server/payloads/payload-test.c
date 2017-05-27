int memcpy(char *Out, char *In, int Len);

long main(char *Data)
{
	memcpy(Data, "This is a test\x00", 14);
	return 14;
}
