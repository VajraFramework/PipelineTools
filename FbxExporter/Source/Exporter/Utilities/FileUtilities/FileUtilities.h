#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Utilities/FileUtilities/dirent.h"

#include <string>
#include <vector>

namespace FileUtilities {
	void CreateFolder(std::string pathToFolder);
	std::vector<std::string> GetFileNamesInFolder(std::string pathToFolder);
	bool FileExistsInFolder(std::string pathToFolder, std::string fileName);
};

#endif // FILE_UTILITIES_H
