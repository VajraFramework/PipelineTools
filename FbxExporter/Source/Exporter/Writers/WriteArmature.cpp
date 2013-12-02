#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Writers/WriteArmature.h"

void exportBone(Bone* bone, std::ofstream& file) {
	printf("\nExporting bone: %s", bone->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open armature file for adding bone %s", bone->name.c_str());
	}

	file << bone->name << "\n";
	file << bone->idx  << "\n";

	if (bone->parentName == "") {
		file << "NULL" << "\n";
	} else {
		file << bone->parentName << "\n";
	}

	file << bone->childrenNames.size() << "\n";
	for (int i = 0; i < bone->childrenNames.size(); ++i) {
		file << bone->childrenNames[i] << " ";
	}
	file << "\n";

	WriteGlmMat4x4ToFile(bone->bindPoseMatrix, file);
}

void exportArmature(Armature* armature, std::ofstream& file) {
	printf("\nExporting armature for armature: %s", armature->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open armature file for armature %s", armature->name.c_str());
	}

	file << armature->bones.size() << "\n";

	for (auto bone_it = armature->bones.begin(); bone_it != armature->bones.end(); ++bone_it) {
		Bone* bone = bone_it->second;
		exportBone(bone, file);
	}

	file << armature->rootBoneName << "\n";
}
