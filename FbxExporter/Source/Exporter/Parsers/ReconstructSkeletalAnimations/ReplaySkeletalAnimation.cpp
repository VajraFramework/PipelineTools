#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Definitions/SkeletalAnimationData.h"
#include "Exporter/Definitions/Vertex.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Parsers/ReconstructSkeletalAnimations/ReplaySkeletalAnimation.h"
#include "Exporter/Utilities/Utilities.h"

#include <algorithm>
#include <fstream>
#include <vector>

void replayLocalRotationsAtTimeForBone_recursive(Armature* armature, Bone* trootBone, Scene* scene, float time) {

	trootBone->ResetLocalRotations();

	Model* trootBoneModel = scene->GetModelByModelName(trootBone->name);
	ASSERT(trootBoneModel != nullptr, "Found model for bone %s", trootBone->name.c_str());

	// TODO [Implement] Support more than 1 set of rigid animations on each bone, maybe
	ASSERT(trootBoneModel->rigidAnimationDatas->size() <= 1, "Not more than 1 set of rigid animations per bone");
	//
	if (trootBoneModel->rigidAnimationDatas->size() != 0) {
		RigidAnimationData* rigidAnimationData = trootBoneModel->rigidAnimationDatas->at(0);
		if (rigidAnimationData->hasKeyFrameAtTime(time)) {
			RigidAnimationKeyframe* rigidKeyframe = rigidAnimationData->GetExistingKeyframeAtTime(time);
			glm::vec3 rotation = rigidKeyframe->rotation;
			trootBone->Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f), true);
			trootBone->Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f), true);
			trootBone->Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f), true);
		}
	}

	// Do the same for children bones:
	for (auto it = trootBone->childrenNames.begin(); it != trootBone->childrenNames.end(); ++it) {
		std::string childBoneName = *it;
		Bone* childBone = armature->GetBoneByName(childBoneName);
		if (childBone != nullptr) {
			replayLocalRotationsAtTimeForBone_recursive(armature, childBone, scene, time);
		}
	}
}

void replayLocalRotationsAtTime(Armature* armature, Scene* scene, float time) {
	Bone* rootBone = armature->GetBoneByName(armature->GetRootBoneName());
	ASSERT(rootBone != nullptr, "Found root bone");

	replayLocalRotationsAtTimeForBone_recursive(armature, rootBone, scene, time);
}

void updateFinalBoneMatricesForArmature(Armature* armature, Scene* scene) {
	armature->UpdateFinalBoneMatrices();
}

void updateSkeletalAnimationDataFromArmatureForTime(Armature* armature, Scene* scene, float time) {
	SkeletalAnimationData* skeletalAnimationData = armature->skeletalAnimationData;
	for (auto it = armature->bones.begin(); it != armature->bones.end(); ++it) {
		Bone* bone = it->second;
		skeletalAnimationData->AddBakedBoneMatrixToKeyframe(armature->finalBoneTransforms[bone->idx], bone->idx, time);
	}
}

void ReplaySkeletalAnimationIntoSkeletalAnimationData(Armature* armature, Scene* scene, std::vector<float>& timesOfKeyframes) {
	armature->skeletalAnimationData = new SkeletalAnimationData();

	// TODO [Implement] Come up with a better naming scheme for skeletal animation clips based on the actual clips which will be specified by the artist in terms of keyframe numbers in a seperate file
	armature->skeletalAnimationData->SetName(armature->name);

	for (int i = 0; i < timesOfKeyframes.size(); ++i) {
		float time = timesOfKeyframes[i];
		replayLocalRotationsAtTime(armature, scene, time);

		// Update final bone matrices
		updateFinalBoneMatricesForArmature(armature, scene);

		// Add it to skeletal animation data
		updateSkeletalAnimationDataFromArmatureForTime(armature, scene, time);
	}
}
