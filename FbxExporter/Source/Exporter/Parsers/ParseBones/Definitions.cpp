#include "Exporter/Definitions/SkeletalAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Utilities/Utilities.h"

Bone::Bone() {
	this->idx = -1;
}

Bone::~Bone() {
}

void Bone::SetName(std::string name) {
	this->name = name;
	size_t colonPos = this->name.find(":");
	while (colonPos != std::string::npos) {
		this->name.replace(colonPos, 1, "_");
		colonPos = this->name.find(":");
	}
}

void Bone::AddChild(std::string childBoneName) {
	for (auto it = this->childrenNames.begin(); it != this->childrenNames.end(); ++it) {
		std::string eachChildBoneName = *it;
		ASSERT(eachChildBoneName != childBoneName, "Not duplicate child bone being added (name = %s)", childBoneName.c_str());
	}
	this->childrenNames.push_back(childBoneName);
}

void Bone::SetParent(std::string parentBoneName) {
	this->parentName = parentBoneName;
}

void Armature::SetName(std::string name) {
	this->name = name;
	size_t colonPos = this->name.find(":");
	while (colonPos != std::string::npos) {
		this->name.replace(colonPos, 1, "_");
		colonPos = this->name.find(":");
	}
}

void Armature::AddBone(Bone* bone) {
	ASSERT(this->bones.find(bone->name) == this->bones.end(), "Not duplicate bone being added (name = %s)", bone->name.c_str());

	this->bones[bone->name] = bone;
}

Bone* Armature::GetBoneByName(std::string boneName) {
	ASSERT(this->bones.find(boneName) != this->bones.end(), "Bone found (name = %s)", boneName.c_str());
	return this->bones[boneName];
}

bool Armature::HasBoneByName(std::string boneName) {
	return (this->bones.find(boneName) != this->bones.end());
}

Armature::Armature() {
	this->skeletalAnimationData = nullptr;
}

Armature::~Armature() {
	if (this->skeletalAnimationData != nullptr) {
		delete this->skeletalAnimationData;
	}
}
