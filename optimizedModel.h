#pragma once
#include <iostream>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <tuple>
#include <vector>
#include <span>
#include "Symbol.h"

namespace mod2 {
	using USHORT = unsigned short;
	unsigned short total = 0;
	constexpr int END_OF_STREAM = 256;
	constexpr int TREE_SIZE = 258;
	constexpr int NO_OF_LEAFS = 257;
	float timer = 0;
	void exittHandler() {
		printf("\ntotal update time = %f\n", timer);
	}

	struct Tree {
		struct Node {
			char symbol = 0;
			unsigned char count = 0;
			USHORT leftChildCumCount = 0;
			bool operator > (const Node& other) {
				return this->count > other.count;
			}
			bool operator < (const Node& other) {
				return this->count < other.count;
			}
			bool operator == (const Node& other) {
				return this->symbol == other.symbol;
			}
			bool operator != (const Node& other) {
				return !(*this == other);
			}
		};
		std::vector<Node> node;
		std::vector<int> leaf;
	};

	Tree tree;

	void initializeModel() {
		tree.node.clear(); tree.node.resize(TREE_SIZE);
		tree.leaf.clear(); tree.leaf.resize(NO_OF_LEAFS);
		total = 0;
		for (int index : std::ranges::iota_view(1, TREE_SIZE)) {//we dont use index 0
			tree.node[index].count = 1;
			if (index <= END_OF_STREAM)
				tree.node[index].symbol = (unsigned char)(index-1);
			tree.leaf[index-1] = index;
			total++;
		}
	}

	USHORT getSum(int i) {
		USHORT result = 0;
		int counter = -1;
		USHORT block = 0;
		USHORT shiftLeft = 0;
		USHORT multiplier = 2;
		while (1) {
			int childNode = multiplier * i;
			++counter;
			block = 1 << counter;
			if (childNode < TREE_SIZE) {
				for (int n{ 0 }; n < block; ++n) {
					if ((childNode + n) == TREE_SIZE)
						return result;
					result += tree.node[(childNode + n)].count;
				}
				multiplier <<= 1;
			}
			else
				return result;
		}
	}

	void calculateLeftSubtree(int i) {
		tree.node[i].leftChildCumCount = getSum(i);
	}

	template <typename T>
	void swap(T& val1, T& val2) {
		T temp = val1;
		val1 = val2;
		val2 = temp;
	}

	void sort(int c) {
		int pos = tree.leaf[c];
		int j = pos;
		for (; j > 1 && tree.node[j - 1] < tree.node[pos]; --j);
		if (j != pos) {
			swap(tree.node[j], tree.node[pos]);
			swap(tree.leaf[j-1], tree.leaf[pos-1]);
		}
		for (int i = 257; i > 0; --i)
			calculateLeftSubtree(i);
	}

	int getLowerLimit(int position) {
		int remainder{};
		int lowerLimit{ tree.node[position].leftChildCumCount };
		while (position > 1) {
			remainder = position % 2;
			position /= 2;
			if (remainder == 1) {
				lowerLimit += (tree.node[position].leftChildCumCount + tree.node[position].count);
			}
		}
		return lowerLimit;
	}

	std::tuple<USHORT, USHORT> range(int symbol) {
		int upperLimit{}, lowerLimit{};
		int position = tree.leaf[symbol];
		lowerLimit = getLowerLimit(position);
		upperLimit = lowerLimit + tree.node[position].count;
		return std::tuple<USHORT, USHORT>{lowerLimit, upperLimit};
	}

	void updateModel(int c) {
		auto start = std::chrono::high_resolution_clock::now();
		tree.node[tree.leaf[c]].count++;
		++total;
		if (tree.node[1].count >= 255) {
			total = 0;
			std::for_each(std::begin(tree.node) + 1, std::end(tree.node), [](Tree::Node& node) {
				node.count = (node.count + 1) / 2;
				total += node.count;
			});
		}
		//scale counts if they are greater than max size
		if (total > 16383) {
			total = 0;
			std::for_each(std::begin(tree.node) + 1, std::end(tree.node), [](Tree::Node& node) {
				node.count = (node.count + 1) / 2;
				total += node.count;
				});
		}
		sort(c);
		auto stop = std::chrono::high_resolution_clock::now();
		timer += std::chrono::duration<float>(stop - start).count();
	}

	void convertIntToSymbol(int c, Symbol& s) {
		auto [l, h] = range(c);
		s.scale = total;
		s.low_count = USHORT(l);
		s.high_count = USHORT(h);
	}

	void getSymbolScale(Symbol& s) {
		s.scale = total;
	}

	int convertSymbolToInt(long index, Symbol& s) {
		int c{ 0 };
		while (1) {	
			auto [l, h] = range(c);
			if (index < h && index >= l) {
				s.low_count = (USHORT)l;
				s.high_count = (USHORT)h;
				break;
			}
			c++;
		}
		return c;
	}
}