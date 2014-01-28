#ifndef PARSE_BONES_DEFINITIONS_H
#define PARSE_BONES_DEFINITIONS_H


#include "Libraries/glm/glm.hpp"

#include <map>
#include <string>
#include <vector>

#define MAX_BONES 50

// Forward Declarations:
class Armature;
class SkeletalAnimationData;

struct BoneInfluence {
	int controlPointIdx;
	double weight;
};

class Bone {
public:
	Bone(Armature* armature_);
	~Bone();

	int idx;

	void SetName(std::string name);
	std::string name;

	void SetParent(std::string parentBoneName);
	std::string parentName;

	void AddChild(std::string childBoneName);
	std::vector<std::string> childrenNames;

	glm::mat4 GetBindPoseMatrix() { return this->bindPoseMatrix; }
	void SetBindPoseMatrix(glm::mat4 m);

	void ResetLocalRotations();
	void ResetLocalTranslations();
	//
	void Rotate(float angleInDegrees, glm::vec3 axis, bool boneSpace = false);
	void Translate(float distance, glm::vec3 along, bool boneSpace = false);


	glm::mat4 toWorldMatrix;
	glm::mat4 toBoneMatrix;

	std::vector<BoneInfluence*> influences;

private:
	void init();
	void destroy();

	void updateBoneMatricesRecursive();
	void propogateRawMatrixToChildren(glm::mat4 rawMatrix);

	glm::mat4 bindPoseMatrix;
	//
	glm::mat4 localRotationMatrix;
	glm::mat4 localTranslationMatrix;
	glm::mat4 localMatrixCumulative;

	Armature* armature;

	friend class Armature;

};

class Armature {
public:
	Armature();
	~Armature();

	void SetName(std::string name);
	std::string name;

	void AddBone(Bone* bone);
	Bone* GetBoneByName(std::string boneName);
	bool HasBoneByName(std::string boneName);

	void SetRootBoneName(std::string rootBoneName) { this->rootBoneName = rootBoneName; }
	std::string GetRootBoneName() { return this->rootBoneName; }

	void UpdateFinalBoneMatrices();

	std::map<std::string /* bone name */, Bone*> bones;
	std::string rootBoneName;

	SkeletalAnimationData* skeletalAnimationData;

	glm::mat4 finalBoneTransforms[MAX_BONES];

private:
};


#endif // PARSE_BONES_DEFINITIONS_H
