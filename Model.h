#pragma once
using USHORT = unsigned short;
#define END_OF_STREAM 256
#define END_OF_COUNT 0
float timer = 0;


void exittHandler() {
	printf("\ntotal update time = %f\n", timer);
}
struct Symbol {
	USHORT low_count;
	USHORT high_count;
	USHORT scale;
};

USHORT totals[258];

void initializeModel(unsigned char counts[]) {
	totals[0] = 0;
	for (unsigned char index : std::ranges::iota_view(0, END_OF_STREAM)) {
		totals[index + 1] = totals[index] + counts[index];
	}
	totals[END_OF_STREAM + 1] = totals[END_OF_STREAM] + 1;
}


void updateModel(int c, unsigned char counts[]) {
	auto start = std::chrono::high_resolution_clock::now();
	unsigned total{ 0 };
	USHORT scale{1};
	counts[c]++;
	if (counts[c] == 255) {
		std::for_each(counts, counts + 256, [](unsigned char& count) {count = (count + 1) / 2; });
	}
	total = std::accumulate(counts, counts + 256, 1);//start from one to account for EOF symbol
	if (total > (32767 - 256))
		scale = 4;
	else if (total > 16383)
		scale = 2;
	
	for (int i = 0; i < 256; ++i) {
		counts[i] /= scale;
		totals[i + 1] = totals[i] + counts[i];
	}
	totals[END_OF_STREAM + 1] = totals[END_OF_STREAM] + 1;
	auto stop = std::chrono::high_resolution_clock::now();
	timer += std::chrono::duration<float>(stop - start).count();
}