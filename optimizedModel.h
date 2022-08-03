#pragma once
#include <tuple>

namespace mod2 {
	using USHORT = unsigned short;
	constexpr int END_OF_STREAM = 257;
	float timer = 0;
	void exittHandler() {
		printf("\ntotal update time = %f\n", timer);
	}

	struct Node {
		char symbol = 0;
		unsigned char count = 0;
		USHORT leftChildCumCount = 0;
		Node& operator= (const Node& other) {
			if (this != &other) {
				this->count = other.count;
				this->leftChildCumCount = other.leftChildCumCount;
				this->symbol = other.symbol;
			}
			return *this;
		}
		bool operator > (const Node& other) {
			return this->count > other.count;
		}
		bool operator < (const Node& other) {
			return this->count > other.count;
		}
		bool operator == (const Node& other) {
			return this->symbol == other.symbol;
		}
		bool operator != (const Node& other) {
			return !(*this == other);
		}
	};

	std::vector<Node> tree(258);

	void initializeModel() {
		//we dont use index 0
		for (unsigned char index : std::ranges::iota_view(1, END_OF_STREAM + 1)) {
			tree[index].symbol = (char)index;
			tree[index].count = 1;
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
			if (childNode < END_OF_STREAM) {
				for (int n{ 0 }; n < block; ++n) {
					if ((childNode + n) == END_OF_STREAM)
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
		for (; i != 0; i /= 2) {
			tree[i].leftChildCumCount = getSum(i);
		}
	}

	void sort(int c) {
		int pos = int(c) + 1;
		int j = pos - 1;
		for (; tree[j] < tree[pos] && j > 0; --j) {}
		j++;
		if (j != pos) {
			Node temp = tree[pos];
			memmove(&tree[j + 1], &tree[j], (pos - j) * sizeof(Node));
			tree[j] = temp;
		}
		calculateLeftSubtree(pos);
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

	std::tuple<int, int> range(char symbol) {
		int upperLimit{}, lowerLimit{};
		int position = 1;
		//find symbol position in tree
		for (auto elem = tree.begin() + 1; elem != tree.end(); ++elem) {
			if (elem->symbol == symbol)
				break;
			++position;
		}
		lowerLimit = getLowerLimit(position);
		upperLimit = lowerLimit + tree[position].count;
		return std::tuple<int, int>{lowerLimit, upperLimit};
	}

	void updateModel(int c) {
		auto start = std::chrono::high_resolution_clock::now();
		unsigned total{ 0 };
		USHORT scale{ 1 };
		tree[c + 1].count++;
		sort(c);
		if (tree[1].count == 255) {
			//257 not 258 cos' we are not resizing the END_OF_STREAM
			std::for_each(std::begin(tree) + 1, std::begin(tree) + 257, [](Node& node) {node.count = (node.count + 1) / 2; });
		}
		//auto [_, h] = range(257);
		auto stop = std::chrono::high_resolution_clock::now();
		timer += std::chrono::duration<float>(stop - start).count();
	}
}