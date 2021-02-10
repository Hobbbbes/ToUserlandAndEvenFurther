#pragma once
#include "Framebuffer.h"
#include "PSF1_Font.h"
#include "EfiMemory.h"

struct BootInfo
{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};