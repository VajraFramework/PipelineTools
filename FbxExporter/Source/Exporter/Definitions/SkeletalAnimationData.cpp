#include "Exporter/Definitions/SkeletalAnimationData.h"
#include "Exporter/Utilities/Utilities.h"

#include <algorithm>


void SkeletalAnimationData::AddBakedBoneMatrixToKeyframe(glm::mat4 bakedBoneMatrix, unsigned int boneId, float time) {

	SkeletalAnimationKeyframe* keyframe = this->getKeyframeAtTime(time);

	VERIFY(keyframe != nullptr, "Got a valid keyframe");

	keyframe->bakedBoneMatrices[boneId].finalBakedMatrix = bakedBoneMatrix;
}

void SkeletalAnimationData::SetName(std::string newName) {
	size_t whitespacePos = std::string::npos;
	while ((whitespacePos = newName.find(' ')) != std::string::npos) {
		newName.replace(whitespacePos, 1, "_");
	}
	this->name = newName;
}

SkeletalAnimationKeyframe* SkeletalAnimationData::GetKeyframeAtIndex(unsigned int index) {
	if (index < this->keyframes.size()) {
		return this->keyframes[index];
	}
	return nullptr;
}



SkeletalAnimationKeyframe* SkeletalAnimationData::getKeyframeAtTime(float time) {
	// Look for existing keyframes at that time:
	for (int i = 0; i < this->keyframes.size(); ++i) {
		SkeletalAnimationKeyframe* keyframe = this->keyframes[i];
		if (areFloatsApproximatelyEqual(time, keyframe->time)) {
			return keyframe;
		}
	}

	// No existing keyframe found. Must add a new one:
	SkeletalAnimationKeyframe* newKeyframe = new SkeletalAnimationKeyframe();
	newKeyframe->time = time;
	// Insert it at the appropriate place:
	auto keyframe_it = this->keyframes.begin();
	for (; keyframe_it != this->keyframes.end(); ++keyframe_it) {
		SkeletalAnimationKeyframe* keyframe = *keyframe_it;
		if (keyframe->time > newKeyframe->time) {
			break;
		}
	}
	this->keyframes.insert(keyframe_it, newKeyframe);

	return newKeyframe;
}
