#include "Exporter/Utilities/FileUtilities/FileUtilities.h"

#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>

void FileUtilities::CreateFolder(std::string pathToFolder) {
	mkdir(pathToFolder.c_str());
}

std::vector<std::string> FileUtilities::GetFileNamesInFolder(std::string pathToFolder) {
	std::vector<std::string> fileNames;

	DIR *dir;
	struct dirent *ent;

	// Get list of models to be converted from collada:
	dir = opendir (pathToFolder.c_str ());
	if (dir != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if (strcmp (ent->d_name, ".") != 0 && strcmp (ent->d_name, "..") != 0) {
				fileNames.push_back (ent->d_name);
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
	}

	return fileNames;
}
