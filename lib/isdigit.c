#include <common.h>

int isdigit(c)
int c;
{
	return (c >= '0' && c <= '9' ? 1 : 0);
}
