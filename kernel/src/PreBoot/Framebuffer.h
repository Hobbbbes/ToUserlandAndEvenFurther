#pragma once
#include <stdint.h>
struct FrameBuffer{
	void * BaseAddress;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelsPerScanLine;
} ;