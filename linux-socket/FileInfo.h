#pragma once

#include <iostream>

struct FileInfo {
	uintmax_t fileSize = 0;
	char fileName[256]{0};
};


std::ostream &operator<<(std::ostream &os, const FileInfo &fileInfo) {
	os << "FileName: " << fileInfo.fileName << "\n";
	os << "FileSize: " << fileInfo.fileSize << "\n";
	return os;
}