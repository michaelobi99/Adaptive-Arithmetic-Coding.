#include <filesystem>
#include <chrono>
#include <format>
#include "Encoder.h"
#include "Decoder.h"

namespace fs = std::filesystem;

struct Timer {
public:
	Timer() = default;
	void Start() {
		start = std::chrono::high_resolution_clock::now();
	}
	void Stop() {
		stop = std::chrono::high_resolution_clock::now();
	}
	float time() {
		elapsedTime = std::chrono::duration<float, std::milli>(stop - start).count();
		return elapsedTime;
	}
private:
	float elapsedTime{};
	std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
};


uintmax_t fileSize(fs::path const& path) {
	auto lengthInbytes = fs::file_size(path);
	return lengthInbytes;
}

int main() {
	auto timer = Timer();
	std::fstream output1(R"(..\AdaptiveArithmeticCoding\testFile3.txt)", std::ios_base::out | std::ios_base::binary);
	std::fstream input(R"(..\AdaptiveArithmeticCoding\testFile.txt)", std::ios_base::in | std::ios_base::binary);
	if (!input.is_open())
		exit(1);
	auto output = stl::OpenOutputBitFile(R"(..\AdaptiveArithmeticCoding\testFile2.txt)");
	auto input1 = stl::OpenInputBitFile(R"(..\AdaptiveArithmeticCoding\testFile2.txt)");

	try {
		std::cout << "compression started....\n";
		timer.Start();
		compressFile(input, output);
		timer.Stop();
		std::cout << "\nFile compression complete\n";
		printf("Arithmetic coding compression time = %f milliseconds\n\n", timer.time());
		stl::closeOutputBitFile(output);

		std::cout << "Expansion started....\n";
		timer.Start();
		expandFile(input1, output1);
		timer.Stop();
		std::cout << "\nFile expansion complete\n";
		printf("Arithmetic coding expansion time = %f milliseconds\n\n", timer.time());
		stl::closeInputBitFile(input1);
		output1.close();

		//print file sizes
		std::cout << std::format("Original file size = {}bytes\n", fileSize(fs::path(R"(..\AdaptiveArithmeticCoding\testFile.txt)")));
		std::cout << std::format("Compressed file size = {} bytes\n", fileSize(fs::path(R"(..\AdaptiveArithmeticCoding\testFile2.txt)")));
		std::cout << std::format("Expanded file size = {} bytes\n", fileSize(fs::path(R"(..\AdaptiveArithmeticCoding\testFile3.txt)")));
	}
	catch (stl::FileError const& error) {
		std::cout << error.what();
		std::cout << "File compression failed\n";
	}
	catch (...) {
		std::cout << "An error occurred during compression or expansion\n";
	}
}