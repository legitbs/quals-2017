#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// just a helper prog to take the hsq output and put it into a suitable C format

int main(int argc, char **argv) {
	FILE *in;
	char buf[6];
	unsigned int total_bytes = 0;
	short value;
	int first = 1;
//	unsigned short ProgLen = 0;

	if (argc != 2) {
		printf("sq2c <input file>\n");
		exit(0);
	}

	if ((in = fopen(argv[1], "r")) == NULL) {
		printf("unable to open file %s\n", argv[1]);
		exit(0);
	}

	printf("short PROG[32768] = {");
	while (fread(buf+total_bytes, 1, 1, in) != 0) {
		if (buf[total_bytes] == ' ' || buf[total_bytes] == '\n') {
			if (total_bytes == 0) {
				continue;
			}
			buf[total_bytes] = '\0';
//			ProgLen++;
			if (first) {
				printf("%s", buf);
				first = 0;
			} else {
				printf(",%s", buf);
			}
			total_bytes = 0;
		} else {
			total_bytes += 1;
		}
	}
	printf("};\n");
	//printf("unsigned short ProgLen = %d;\n", ProgLen);
	printf("unsigned short ProgLen = 32767;\n");

	fclose(in);
}
