#pragma once
#include <iostream>
#include <memory>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <format>
#include <ranges>
#include "BitIO.h"
#include <string>
#include "Model.h"
#include <bitset>

const char* compressionName = "Adaptive order-0 model with arithmetic coding\n";
const char* usage = "inputFile outputFile\n";


void convertIntToSymbol(int c, Symbol& s) {
	s.scale = totals[END_OF_STREAM + 1];
	s.low_count = totals[c];
	s.high_count = totals[c + 1];
}

void encodeSymbol(std::unique_ptr<stl::BitFile>& output, Symbol& s, USHORT& low, USHORT& high, USHORT& underflowBits) {
	unsigned long range = (high - low) + 1;
	high = low + static_cast<USHORT>((range * s.high_count) / s.scale - 1);
	low = low + static_cast<USHORT>((range * s.low_count) / s.scale);
	//the following loop churns out new bits until high and low are far enough apart to have stabilized
	for (;;) {
		//if their MSBs are the same
		if ((high & 0x8000) == (low & 0x8000)) {
			stl::outputBit(output, high & 0x8000);
			while (underflowBits > 0) {
				stl::outputBit(output, (~high) & 0x8000);
				underflowBits--;
			}
		}
		//if low first and second MSBs are 01 and high first and second MSBs are 10, and underflow is about to occur
		else if ((low & 0x4000) && !(high & 0x4000)) {
			underflowBits++;
			//toggle the second MSB in both low and high.
			//the shifting operation at the end of the loop will set things right
			high |= (1 << 14);
			low &= ~(1 << 14);
		}
		else {
			return;
		}
		low <<= 1;
		high <<= 1;
		high |= 1;
	}
}

void flushArithmeticEncoder(std::unique_ptr<stl::BitFile>& output, USHORT high, USHORT& underflowBits) {
	stl::outputBit(output, high & 0x8000);
	++underflowBits;
	while (underflowBits > 0) {
		stl::outputBit(output, ~high & 0x8000);
		underflowBits--;
	}
}

size_t readBytes(std::fstream& input, std::string& buffer, size_t limit) {
	buffer.clear();
	buffer.reserve(limit);
	size_t counter = 0;
	int ch{};
	while (counter < limit) {
		ch = input.get();
		if (ch == EOF)
			break;
		buffer.push_back((char)ch);
		++counter;
	}
	return counter;
}

void compressFile(std::fstream& input, std::unique_ptr<stl::BitFile>& output) {
	int c{};
	USHORT low{ 0 }, high{ 0xffff }, underflowBits{ 0 };
	Symbol s;
	unsigned char counts[256];
	for (auto& elem : counts)
		elem = 1;
	initializeModel(counts);
	size_t bufferSize = 4096;
	std::string buffer;
	size_t numBytesRead = 0;
	do {
		numBytesRead = readBytes(input, buffer, bufferSize);
		for (auto c : buffer) {
			convertIntToSymbol(c, s);
			encodeSymbol(output, s, low, high, underflowBits);
			updateModel(c, counts);
		}
	} while (numBytesRead == bufferSize);
	convertIntToSymbol(END_OF_STREAM, s);
	encodeSymbol(output, s, low, high, underflowBits);
	flushArithmeticEncoder(output, high, underflowBits);
	stl::outputBits(output, 0L, 16);
}