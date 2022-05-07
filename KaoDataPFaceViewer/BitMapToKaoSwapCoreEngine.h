#pragma once
#include <windows.h>
#include <vector>
#include <cstdint>


using namespace std;


union RESULT_8BMP_PIXEL {
	uint64_t result;
	byte at[8];
};

RESULT_8BMP_PIXEL Cnv8TSPixelTo8BmpPixel(int iTsColorPackedIndex);
