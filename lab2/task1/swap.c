#include "swap.h"

void Swap(char *left, char *right)
{
	char mid = *left;
	*left = *right;
	*right = mid;
}
