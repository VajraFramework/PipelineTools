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

	file << CLIPNAME_STRING << "#" << skeletalAnimationData->GetName() << "\n";

	int numKeyframes = skeletalAnimationData->GetNumKeyframes();
	file << numKeyframes << "\n\n";

	for (int i = 0; i < numKeyframes; ++i) {
		exportBakedSkeletalAnimationKeyframe(armature, skeletalAnimationData->GetKeyframeAtIndex(i), file);
	}

}
