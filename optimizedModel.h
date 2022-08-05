#pragma once
#include <iostream>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <tuple>
#include <vector>
#include <fstream>
#include "Symbol.h"

namespace mod2 {
	using USHORT = unsigned short;
	unsigned short total = 0;
	constexpr int TREE_SIZE = 258;
	float timer = 0;
	void exittHandler() {
		printf("\ntotal update time = %f\n", timer);
	}

	struct Node {
		int symbol = 0;
		unsigned char count = 0;
		USHORT leftChildCumCount = 0;
		bool operator < (const Node& other) {
			return this->count < other.count;
		}
	};

	std::vector<Node> tree(TREE_SIZE);

	USHORT getSum(int i) {
		USHORT result = 0;
		int counter = -1;
		USHORT block = 0;
		USHORT multiplier = 2;
		while (1) {
			int childNode = multiplier * i;
			++counter;
			block = 1 << counter;
			if (childNode < TREE_SIZE) {
				for (int n{ 0 }; n < block; ++n) {
					if ((childNode + n) == TREE_SIZE)
						return result;
					result += tree[(childNode + n)].count;
				}
				multiplier <<= 1;
			}
			else
				return result;
		}
	}

	void calculateLeftSubtree(int i) {
		tree[i].leftChildCumCount = getSum(i);
	}

	void initializeModel() {
		//we dont use index 0
		total = 0;
		for (int index = 1; index < TREE_SIZE; ++index) {
			tree[index].symbol = (index-1);
			tree[index].count = 1;
			total += 1;
		}
		for (int i = 1; i < TREE_SIZE; ++i)
			calculateLeftSubtree(i);
	}

	int searchPosition(int symbol) {
		int position = 1;
		//find symbol position in tree
		for (int index = 1; index < TREE_SIZE; ++index) {
			if (tree[index].symbol == symbol)
				break;
			++position;
		}
		return position;
	}

	void sort(int pos) {
		int j = pos;
		for (; j > 1 && tree[j - 1] < tree[pos]; --j) {}
		if (j != pos)
			std::swap(tree[pos], tree[j]);
		for (int i = pos; i > 0; --i)
			calculateLeftSubtree(i);
	}


	int getLowerLimit(int position) {
		int remainder{};
		int lowerLimit{ tree[position].leftChildCumCount };
		while (position > 1) {
			remainder = position % 2;
			position /= 2;
			if (remainder == 1) {
				lowerLimit += tree[position].leftChildCumCount;
				lowerLimit += tree[position].count;
			}
		}
		return lowerLimit;
	}

	std::tuple<USHORT, USHORT> range(int symbol) {
		int upperLimit{}, lowerLimit{};
		int position = searchPosition(symbol);
		lowerLimit = getLowerLimit(position);
		upperLimit = lowerLimit + tree[position].count;
		return std::tuple<USHORT, USHORT>{lowerLimit, upperLimit};
	}

	void updateModel(int c) {
		auto start = std::chrono::high_resolution_clock::now();
		int pos = searchPosition(c);
		if (tree[pos].count == 255) {
			total = 0;
			for (int index = 1; index < TREE_SIZE; ++index) {
				tree[index].count = (tree[index].count + 1) / 2;
				total += tree[index].count;
			}
			for (int i = 1; i < TREE_SIZE; ++i)
				calculateLeftSubtree(i);
		}
		if (total == MAX_SIZE) {
			total = 0;
			for (int index = 1; index < TREE_SIZE; ++index) {
				tree[index].count /= 2;
				total += tree[index].count;
			}
			for (int i = 1; i < TREE_SIZE; ++i)
				calculateLeftSubtree(i);
		}
		tree[pos].count++;
		++total;
		sort(pos);
		auto stop = std::chrono::high_resolution_clock::now();
		timer += std::chrono::duration<float>(stop - start).count();
	}

	void convertIntToSymbol(int c, Symbol& s) {
		auto [l, h] = range(c);
		s.scale = total;
		s.low_count = l;
		s.high_count = h;
	}

	void getSymbolScale(Symbol& s) {
		s.scale = total;
	}

	int convertSymbolToInt(long index, Symbol& s) {
		int mid = 1;
		while (true) {
			int low = getLowerLimit(mid);
			int high = tree[mid].count + low;
			if (index >= low && high > index) {
				s.low_count = low;
				s.high_count = high;
				return tree[mid].symbol;
			}
			else if (index >= high)
				mid = (mid << 1) + 1;
			else if (low >= index)
				mid <<= 1;
		}
		
	}
	/*int convertSymbolToInt(long index, Symbol& s) {
		int idx = 0;
		while (1) {
			auto [l, h] = range(idx);
			if (index >= l && h > index) {
				s.low_count = l;
				s.high_count = h;
				return idx;
			}
			++idx;
		}
	}*/
}