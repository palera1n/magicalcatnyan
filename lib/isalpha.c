#include <common.h>

int isalpha(c)
int c;
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
}
