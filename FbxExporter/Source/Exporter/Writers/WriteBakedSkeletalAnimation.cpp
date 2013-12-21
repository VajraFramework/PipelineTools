#include "Exporter/Definitions/AnimClipKeyFrames.h"
#include "Exporter/Definitions/Declarations.h"
#include "Exporter/Definitions/SkeletalAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Writers/WriteBakedSkeletalAnimation.h"

void exportBakedSkeletalAnimationKeyframe(Armature* armature, SkeletalAnimationKeyframe* keyframe, std::ofstream& file) {
	printf("\nExporting keyframe at time %f of baked skeletal animation for armature: %s", keyframe->time, armature->name.c_str());
	
	if (!file.good()) {
		VERIFY(0, "Failed to open skeletal animation file for armature %s", armature->name.c_str());
	}

	file << keyframe->time << "\n";

	int numBakedBoneMatrices = keyframe->bakedBoneMatrices.size();
	ASSERT(numBakedBoneMatrices == armature->bones.size(), "Equal number of bones in armature and keyframe");

	for (auto it = keyframe->bakedBoneMatrices.begin(); it != keyframe->bakedBoneMatrices.end(); ++it) {
		glm::mat4x4 m = it->second.finalBakedMatrix;
		WriteGlmMat4x4ToFile(m, file);
	}
}

std::vector<AnimClipKeyframes*> extractAnimclipKeyframeNumbersFromFile(std::string filePath) {
	std::vector<AnimClipKeyframes*> vectorOfAnimclipKeyframes;

	std::ifstream file(filePath.c_str());
	ASSERT(file.good(), "Able to open file %s, for reading", filePath.c_str());

	int numAnimclips;
	file >> numAnimclips;
	printf("\nAnimclipKeyframes file %s, has %d anim clips", filePath.c_str(), numAnimclips);

	for (unsigned int i = 0; i < numAnimclips; ++i) {
		AnimClipKeyframes* animclipKeyframes = new AnimClipKeyframes();
		//
		file >> animclipKeyframes->clipName;
		file >> animclipKeyframes->startKeyframeNumber >> animclipKeyframes->endKeyframeNumber;
		//
		vectorOfAnimclipKeyframes.push_back(animclipKeyframes);
	}

	return vectorOfAnimclipKeyframes;
}

void exportBakedSkeletalAnimation(Armature* armature, std::ofstream& file) {
	printf("\nExporting baked skeletal animation for armature: %s", armature->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open skeletal animation file for armature %s", armature->name.c_str());
	}

	file << ANIMATION_TYPE_STRING_BAKED_SKELETAL << "\n\n";

	ASSERT(armature->skeletalAnimationData != nullptr, "Armature has skeletal animation data");
	SkeletalAnimationData* skeletalAnimationData = armature->skeletalAnimationData;

	int numBones = armature->bones.size();
	file << numBones << "\n\n";

	// Get the names of the animation clips and their respective keyframes:
	std::string animclipKeyframesFilePath = IMPORT_PATH;
	animclipKeyframesFilePath += armature->name;
	animclipKeyframesFilePath += ".animclipframes";
	//
	std::vector<AnimClipKeyframes*> vectorOfAnimclipKeyframes = extractAnimclipKeyframeNumbersFromFile(animclipKeyframesFilePath);

	for (unsigned int i = 0; i < vectorOfAnimclipKeyframes.size(); ++i) {

		file << CLIPNAME_STRING << "#" << vectorOfAnimclipKeyframes[i]->clipName << "\n";

		int numKeyframes = vectorOfAnimclipKeyframes[i]->endKeyframeNumber - vectorOfAnimclipKeyframes[i]->startKeyframeNumber + 1;
		file << numKeyframes << "\n\n";

		ASSERT(vectorOfAnimclipKeyframes[i]->startKeyframeNumber <= skeletalAnimationData->GetNumKeyframes(), "Valid start keyframe number: %d", vectorOfAnimclipKeyframes[i]->startKeyframeNumber);
		ASSERT(vectorOfAnimclipKeyframes[i]->endKeyframeNumber <= skeletalAnimationData->GetNumKeyframes(), "Valid end keyframe number: %d", vectorOfAnimclipKeyframes[i]->endKeyframeNumber);

		for (int frameNumber = vectorOfAnimclipKeyframes[i]->startKeyframeNumber; frameNumber <= vectorOfAnimclipKeyframes[i]->endKeyframeNumber; ++frameNumber) {
			// Note that frameNumbers start from 1 instead of 0, adjust accordingly:
			int frameNumberIdx = frameNumber - 1;
			//
			exportBakedSkeletalAnimationKeyframe(armature, skeletalAnimationData->GetKeyframeAtIndex(frameNumberIdx), file);
		}
	}

}
