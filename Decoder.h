#pragma once

#include <iostream>
#include <memory>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <format>
#include "BitIO.h"
#include "Model.h"


void initializeArithmeticDecoder(std::unique_ptr<stl::BitFile>& input, USHORT& code) {
	for (int i{ 0 }; i < 16; ++i) {
		code <<= 1;
		code |= stl::inputBit(input);
	}
}

void getSymbolScale(Symbol& s) {
	s.scale = totals[END_OF_STREAM + 1];
}

long getCurrentIndex(Symbol& s, USHORT low, USHORT high, USHORT code) {
	long range{ high - low + 1 };
	long index = (long)(((code - low) + 1) * s.scale - 1) / range;
	return index;
}

int convertSymbolToInt(long index, Symbol& s) {
	int c{};
	for (c = END_OF_STREAM; index < totals[c]; c--) {}
	s.high_count = totals[c + 1];
	s.low_count = totals[c];
	return c;
}

void removeSymbolFromStream(std::unique_ptr<stl::BitFile>& input, Symbol& s, USHORT& low, USHORT& high, USHORT& code) {
	long range{ (high - low) + 1 };
	high = low + (USHORT)((range * s.high_count) / s.scale - 1);
	low = low + (USHORT)((range * s.low_count) / s.scale);
	for (;;) {
		if ((high & 0x8000) == (low & 0x8000)) {
			//do nothing
		}
		else if ((low & 0x4000) && !(high & 0x4000)) {
			code ^= 0x4000;
			/*low &= 0x3fff;
			high |= 0x4000;*/
			high |= (1 << 14);
			low &= ~(1 << 14);
		}
		else
			return;
		low <<= 1;
		high <<= 1;
		high |= 1;
		code <<= 1;
		code |= stl::inputBit(input);
	}
}


void expandFile(std::unique_ptr<stl::BitFile>& input, std::fstream& output) {
	Symbol s;
	int c{};
	USHORT low{ 0 }, high{ 0xffff }, code{ 0 };
	long index{ 0 };
	unsigned char counts[256];
	for (auto& elem : counts)
		elem = 1;
	buildModel(counts);
	initializeArithmeticDecoder(input, code);
	for (;;) {
		getSymbolScale(s);
		index = getCurrentIndex(s, low, high, code);
		c = convertSymbolToInt(index, s);
		if (c == END_OF_STREAM)
			break;
		removeSymbolFromStream(input, s, low, high, code);
		output.put(c);
		updateModel(c, counts);
	}
}