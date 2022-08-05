#pragma once
const int MAX_SIZE = (1 << 14) - 1;
const int END_OF_STREAM = 256;
struct Symbol {
	unsigned short low_count;
	unsigned short high_count;
	unsigned short scale;
};