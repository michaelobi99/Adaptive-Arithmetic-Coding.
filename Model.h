#pragma once
#include <iostream>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <chrono>
#include "Symbol.h"


namespace mod1 {
	using USHORT = unsigned short;
	USHORT totals[258];
	unsigned char counts[256];
	float timer = 0;
	void exittHandler() {
		printf("\ntotal update time = %f\n", timer);
	}

	void initializeModel() {
		for (auto& elem : counts)
			elem = 1;
		totals[0] = 0;
		for (unsigned char index : std::ranges::iota_view(0, END_OF_STREAM)) {
			totals[index + 1] = totals[index] + counts[index];
		}
		totals[END_OF_STREAM + 1] = totals[END_OF_STREAM] + 1;
	}


	void updateModel(int c) {
		auto start = std::chrono::high_resolution_clock::now();
		unsigned total{ 0 };
		USHORT scale{ 1 };
		counts[c]++;
		if (counts[c] == 255) {
			std::for_each(counts, counts + 256, [](unsigned char& count) {count = (count + 1) / 2; });
		}
		total = std::accumulate(counts, counts + 256, 1);//start from one to account for EOF symbo
		if (total > MAX_SIZE)
			scale = 2;

		for (int i = 0; i < 256; ++i) {
			counts[i] /= scale;
			totals[i + 1] = totals[i] + counts[i];
		}
		totals[END_OF_STREAM + 1] = totals[END_OF_STREAM] + 1;
		auto stop = std::chrono::high_resolution_clock::now();
		timer += std::chrono::duration<float>(stop - start).count();
	}

	void convertIntToSymbol(int c, Symbol& s) {
		s.scale = totals[END_OF_STREAM + 1];
		s.low_count = totals[c];
		s.high_count = totals[c + 1];
	}

	void getSymbolScale(Symbol& s) {
		s.scale = totals[END_OF_STREAM + 1];
	}

	int convertSymbolToInt(long index, Symbol& s) {
		int c{};
		for (c = END_OF_STREAM; index < totals[c]; c--) {}
		s.high_count = totals[c + 1];
		s.low_count = totals[c];
		return c;
	}
}
