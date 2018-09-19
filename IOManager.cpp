#include "IOManager.h"

#include <fstream>

bool IOManager::readFileToBuffer(std::string filePath, std::vector<char>& buffer)
{
	//read a file in binary format
	std::ifstream file(filePath, std::ios::binary);
	if (file.fail())
	{
		perror(filePath.c_str());
		return false;
	}

	//seek to the end
	file.seekg(0, std::ios::end);

	//get the file size(how many bytes)
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	//remove file header bytes at the beggining
	fileSize -= file.tellg();

	buffer.resize(fileSize);
	file.read(&(buffer[0]), fileSize);
	file.close();

	return true;
}
