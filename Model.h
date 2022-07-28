#pragma once
using USHORT = unsigned short;
#define END_OF_STREAM 256
#define END_OF_COUNT 0


struct Symbol {
	USHORT low_count;
	USHORT high_count;
	USHORT scale;
};

USHORT totals[258];

void buildTotals(unsigned char counts[]) {
	totals[0] = 0;
	for (unsigned char index : std::ranges::iota_view(0, END_OF_STREAM)) {
		totals[index + 1] = totals[index] + counts[index];
	}
	totals[END_OF_STREAM + 1] = totals[END_OF_STREAM] + 1;
}

void updateModel(int c, unsigned char counts[]) {
	unsigned total{ 1 };
	USHORT scale{};
	counts[c]++;
	if (counts[c] == 255) {
		for (unsigned char index : std::ranges::iota_view(0, 256)) {
			//if (counts[index] > 0)
			counts[index] = (counts[index] + 1) / 2;
		}
	}
	total += std::accumulate(counts, counts + 256, 0);
	if (total > (32767 - 256))
		scale = 4;
	else if (total > 16383)
		scale = 2;
	if (scale != 0)
		for (int i = 0; i < 256; ++i)
			counts[i] /= scale;
	buildTotals(counts);
}