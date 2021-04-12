#pragma once
#include "ck.h"

typedef struct _IMAGE_DEBUG_DIRECTORY_RAW {
	uint8_t format[4];
	uint8_t PdbSignature[16];
	uint32_t PdbDbiAge;
	uint8_t ImageName[256];
} IMAGE_DEBUG_DIRECTORY_RAW, * PIMAGE_DEBUG_DIRECTORY_RAW;

class PdbFile
{
public:
	PdbFile();
	~PdbFile();

	bool ReadSig(char* module_name, char* OUT pdb_signature);
};

