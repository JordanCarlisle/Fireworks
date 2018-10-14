#pragma once
//class needed to align DirectXMath data types correctly (stops program crashing in release mode)
#include <malloc.h>

class AlignmentClass16
{
public:
	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};