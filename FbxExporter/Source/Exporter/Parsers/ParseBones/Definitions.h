#ifndef PARSE_BONES_DEFINITIONS_H
#define PARSE_BONES_DEFINITIONS_H

#include "Libraries/glm/glm.hpp"

#include <map>
#include <string>
#include <vector>

struct BoneInfluence {
	int controlPointIdx;
	double weight;
};

class Bone {
public:
	Bone();
	~Bone();

	int idx;

	void SetName(std::string name);
	std::string name;

	void SetParent(std::string parentBoneName);
	std::string parentName;

	void AddChild(std::string childBoneName);
	std::vector<std::string> childrenNames;

	glm::mat4x4 bindPoseMatrix;

	std::vector<BoneInfluence*> influences;

private:
};

class Armature {
public:

	void SetName(std::string name);
	std::string name;

	void AddBone(Bone* bone);
	Bone* GetBoneByName(std::string boneName);
	bool HasBoneByName(std::string boneName);

	void SetRootBoneName(std::string rootBoneName) { this->rootBoneName = rootBoneName; }
	std::string GetRootBoneName() { return this->rootBoneName; }

	std::map<std::string /* bone name */, Bone*> bones;
	std::string rootBoneName;

private:
};


#endif // PARSE_BONES_DEFINITIONS_H
