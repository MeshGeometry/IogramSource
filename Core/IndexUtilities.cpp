#include "IndexUtilities.h"

bool IndexInRange(int index, int numItems)
{
	return 0 <= index && index < numItems;
}

bool IndexInRange(unsigned index, unsigned numItems)
{
	return 0 <= index && index < numItems;
}