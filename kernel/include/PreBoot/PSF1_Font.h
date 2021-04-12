#pragma once

#include <stdint.h>
struct PSF1_HEADER{
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
};

struct PSF1_FONT{
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
};