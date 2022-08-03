#pragma once

#include <iostream>
#include <memory>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <format>
#include "BitIO.h"
#include "Symbol.h"
//#include "optimizedModel.h"
#include "Model.h"
using namespace mod1;
//using namespace mod2;

void initializeArithmeticDecoder(std::unique_ptr<stl::BitFile>& input, USHORT& code) {
	for (int i{ 0 }; i < 16; ++i) {
		code <<= 1;
		code |= stl::inputBit(input);
	}
}

long getCurrentIndex(Symbol& s, USHORT low, USHORT high, USHORT code) {
	long range{ high - low + 1 };
	long index = (long)(((code - low) + 1) * s.scale - 1) / range;
	return index;
}

void removeSymbolFromStream(std::unique_ptr<stl::BitFile>& input, Symbol& s, USHORT& low, USHORT& high, USHORT& code) {
	long range{ (high - low) + 1 };
	//std::cout << std::format("{:<7}", low + static_cast<USHORT>((range * s.high_count) / s.scale - 1));
	//std::cout << std::format("{:<7}", low + static_cast<USHORT>((range * s.low_count) / s.scale));
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
	initializeModel();
	initializeArithmeticDecoder(input, code);
	for (;;) {
		getSymbolScale(s);
		index = getCurrentIndex(s, low, high, code);
		c = convertSymbolToInt(index, s);
		if (c == END_OF_STREAM)
			break;
		removeSymbolFromStream(input, s, low, high, code);
		output.put(c);
		updateModel(c);
	}
}