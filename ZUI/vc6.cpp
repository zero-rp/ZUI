#include <stdio.h>
#include <ZUI.h>
#if (defined NDEBUG)
void __cdecl operator delete(void* p, unsigned int)
{
	free(p);
}

void __cdecl operator delete[](void * p, unsigned int)
{
	free(p);
}

#endif