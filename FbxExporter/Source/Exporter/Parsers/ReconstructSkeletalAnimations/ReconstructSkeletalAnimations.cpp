#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Definitions/Vertex.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Parsers/ReconstructSkeletalAnimations/ReconstructSkeletalAnimations.h"
#include "Exporter/Parsers/ReconstructSkeletalAnimations/ReplaySkeletalAnimation.h"
#include "Exporter/Utilities/Utilities.h"

#include <algorithm>
#include <fstream>
#include <vector>

void insertUniqueNumberIntoVector(std::vector<float>& out_vectorOfNumbers, float newUniqueNumber) {
	for (int i = 0; i < out_vectorOfNumbers.size(); ++i) {
		if (areFloatsApproximatelyEqual(out_vectorOfNumbers[i], newUniqueNumber)) {
			// Number already exists:
			return;
		}
	}
	out_vectorOfNumbers.push_back(newUniqueNumber);
}

void getListOfKeyframeTimesForBone_recursive(Armature* armature, Bone* trootBone, Scene* scene, std::vector<float>& out_listOfKeyframeTimes) {
	Model* boneModel = scene->GetModelByModelName(trootBone->name);
	ASSERT(boneModel != nullptr, "Bone model found");

	// TODO [Implement] Support more than 1 set of rigid animations on each bone, maybe
	ASSERT(boneModel->rigidAnimationDatas->size() <= 1, "Not more than 1 set of rigid animations per bone");
	//
	if (boneModel->rigidAnimationDatas->size() != 0) {
		RigidAnimationData* rigidAnimationData = boneModel->rigidAnimationDatas->at(0);
		int numKeyframes = rigidAnimationData->GetNumKeyframes();
		for (int i = 0; i < numKeyframes; ++i) {
			RigidAnimationKeyframe* keyframe = rigidAnimationData->GetKeyframeAtIndex(i);
			insertUniqueNumberIntoVector(out_listOfKeyframeTimes, keyframe->time);
		}
	}

	// Recurse over children bones:
	for (auto it = trootBone->childrenNames.begin(); it != trootBone->childrenNames.end(); ++it) {
		std::string childBoneName = *it;
		Bone* childBone = armature->GetBoneByName(childBoneName);
		if (childBone != nullptr) {
			getListOfKeyframeTimesForBone_recursive(armature, childBone, scene, out_listOfKeyframeTimes);
		}
	}
}

std::vector<float> getListOfKeyframeTimesForArmature(Armature* armature, Scene* scene) {
	std::vector<float> timesOfKeyframes;

	Bone* rootBone = armature->GetBoneByName(armature->GetRootBoneName());
	ASSERT(rootBone != nullptr, "Rootbone found");

	getListOfKeyframeTimesForBone_recursive(armature, rootBone, scene, timesOfKeyframes);

	// Sort thelist of times of keyframes:
	std::sort(timesOfKeyframes.begin(), timesOfKeyframes.end());

	return timesOfKeyframes;
}

void reconstructSkeletalAnimationForModel(Model* model, Scene* scene) {
	ASSERT(model->mesh != nullptr  &&  model->mesh->armature != nullptr, "Model has an armature");

	Armature* armature = model->mesh->armature;

	std::vector<float> timesOfKeyframes;	// Sorted list of the times at which a keyframe occurs for at least 1 bone in the armature
	{
		timesOfKeyframes = getListOfKeyframeTimesForArmature(armature, scene);
		printf("\nGot list of %d times of keyframes for armature %s:\n", timesOfKeyframes.size(), armature->name.c_str());
		for (int i = 0; i < timesOfKeyframes.size(); ++i) {
			printf("%f ", timesOfKeyframes[i]);
		}
	}

	{
		ReplaySkeletalAnimationIntoSkeletalAnimationData(armature, scene, timesOfKeyframes);
	}
}

void ReconstructSkeletalAnimation(Scene* scene) {
	for (auto it = scene->models->begin(); it != scene->models->end(); ++it) {
		Model* model = *it;
		if (model->mesh != nullptr && model->mesh->armature != nullptr) {
			reconstructSkeletalAnimationForModel(model, scene);
		}
	}
	return;
}
