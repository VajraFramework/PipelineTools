#ifndef SKELETAL_ANIMATION_DATA_H
#define SKELETAL_ANIMATION_DATA_H

#include "Libraries/glm/glm.hpp"

#include <map>
#include <string>
#include <vector>

// Forward Declarations:
class SkeletalAnimationKeyframe;
class BakedBoneMatrix;

class SkeletalAnimationData {
public:
	void AddBakedBoneMatrixToKeyframe(glm::mat4 bakedBoneMatrix, unsigned int boneId, float time);

	std::string GetName() { return this->name; }
	void SetName(std::string newName);

	int GetNumKeyframes() { return this->keyframes.size(); }
	SkeletalAnimationKeyframe* GetKeyframeAtIndex(unsigned int index);

private:

	// Utility Functions:
	SkeletalAnimationKeyframe* getKeyframeAtTime(float time);

	std::string name;
	std::vector<SkeletalAnimationKeyframe*> keyframes;
};

class SkeletalAnimationKeyframe {
public:
	SkeletalAnimationKeyframe() {
	}
	~SkeletalAnimationKeyframe() {}

	float time;
	std::map<unsigned int /* bone id */, BakedBoneMatrix> bakedBoneMatrices;

private:
};

class BakedBoneMatrix {
public:
	BakedBoneMatrix() {
		for (int r = 0; r < 4; ++r) {
			for (int c = 0; c < 4; ++c) {
				this->finalBakedMatrix[r][c] = (r == c);
			}
		}
	}
	~BakedBoneMatrix() {}

	glm::mat4 finalBakedMatrix;

private:
};

#endif // SKELETAL_ANIMATION_DATA_H
