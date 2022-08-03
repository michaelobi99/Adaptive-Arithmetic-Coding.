#pragma once

namespace mod1 {
	using USHORT = unsigned short;
	constexpr int END_OF_STREAM = 256;
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
}
