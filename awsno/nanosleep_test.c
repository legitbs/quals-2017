#include <stdio.h>
#include <time.h>

int main()
{
	struct timespec t;

	t.tv_sec = 5;
	t.tv_nsec = 0;

	printf("%d %d\n", t.tv_sec, t.tv_nsec);
	return 0;
}
