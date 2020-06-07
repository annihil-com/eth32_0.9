// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
CPunkbuster Punkbuster;

// This way checksum our eth32.dll's .code section is dynamic, they can't produce static checksum on it
void CPunkbuster::RandomizeDummyFunc()
{
	char *offset = (char *) ((DWORD) DummyFunc);
	char patch[5];
	patch[0] = '\x04';	// ret
	int r = random()*4000000000;
	memcpy(&patch[1], &r, 4);

	unprotect(offset, 5);
	memcpy(offset, patch, 5);
	reprotect(offset, 5);
}

// make sure this bogus func is at least > 5 bytes long or Bad Things happen
void DummyFunc(void)
{
	int i = 0;
	int j[1024];
	float q;

	for (i=0; i<1024; i++){
		q = sinf( (float)i*M_PI);
		j[i] = (int)q - 255/(12+i);
	}
}
