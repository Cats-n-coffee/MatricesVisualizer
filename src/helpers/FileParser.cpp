#include "FileParser.h"

std::string ReadResourceFileToStr(const std::string& filePath)
{
	std::cout << "something something " << filePath << std::endl;

	std::ifstream fileStream(filePath);
	if (!fileStream) {
		printf("file opening failed: ");
		char errorMessage[1024];
		strerror_s(errorMessage, 1024, errno);
		std::cerr << "error: " << errorMessage << std::endl;

		return "Whoopsies";
	}

	fileStream.seekg(0, std::ios::end);
	auto size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);

	std::string output(size, '\0');
	fileStream.read(&output[0], size);

	fileStream.close();

	return output;
}