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

#include "Libraries/glm/glm.hpp"
#include "Libraries/glm/gtc/quaternion.hpp"
#include "Libraries/glm/gtx/quaternion.hpp"

#include <algorithm>
#include <fstream>
#include <vector>

Model* getParentModelForBone(Bone* bone, Scene* scene) {
	FbxNode* parentFbxNode = bone->GetFbxNode()->GetParent();
	if (parentFbxNode != nullptr) {
		std::string parentName = parentFbxNode->GetName();
		// TODO [Cleanup] Move this somewhere else, along with the code in Model::SetName()
		// Clean it up a little:
		size_t colonPos = parentName.find(":");
		while (colonPos != std::string::npos) {
			parentName.replace(colonPos, 1, "_");
			colonPos = parentName.find(":");
		}
		Model* parentModel = scene->GetModelByModelName(parentName.c_str());
		return parentModel;
	}
	return nullptr;
}

glm::quat convertEulerAnglesToQuaternion(float x, float y, float z) {
	glm::quat qx = glm::angleAxis(x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qy = glm::angleAxis(y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qz = glm::angleAxis(z, glm::vec3(0.0f, 0.0f, 1.0f));
	//
	return (qz * qy * qx);
}

glm::vec3 concatenateEulerRotations(glm::vec3 rotation1, glm::vec3 rotation2) {
	glm::quat q1 = convertEulerAnglesToQuaternion(rotation1.x, rotation1.y, rotation1.z);
	glm::quat q2 = convertEulerAnglesToQuaternion(rotation2.x, rotation2.y, rotation2.z);

	glm::vec3 eulerAngles = glm::eulerAngles(q2 * q1);
	return eulerAngles;
}

void replayLocalRotationsAtTimeForBone_recursive(Armature* armature, Bone* trootBone, Scene* scene, float time) {

	trootBone->ResetLocalRotations();
	trootBone->ResetLocalTranslations();

	Model* trootBoneModel = scene->GetModelByModelName(trootBone->name);
	ASSERT(trootBoneModel != nullptr, "Found model for bone %s", trootBone->name.c_str());

	// TODO [Implement] Support more than 1 set of rigid animations on each bone, maybe
	ASSERT(trootBoneModel->rigidAnimationDatas->size() <= 1, "Not more than 1 set of rigid animations per bone");
	//
	if (trootBoneModel->rigidAnimationDatas->size() != 0) {
		RigidAnimationData* rigidAnimationData = trootBoneModel->rigidAnimationDatas->at(0);
		if (rigidAnimationData->hasKeyFrameAtTime(time)) {
			RigidAnimationKeyframe* rigidKeyframe = rigidAnimationData->GetExistingKeyframeAtTime(time);
			//
			// The translation in the rigid key frame is stored in the parent bone's co-ordinate space,
			// but we want it in local space
			glm::vec4 total_translation = glm::vec4(rigidKeyframe->translation.x, rigidKeyframe->translation.y, rigidKeyframe->translation.z, 1.0f);
#if 1
			// So, we transform the translation by the inverse of the parent's bind pose matrix:
			glm::vec4 translation_to_apply;
			if (trootBone->parentName != "") {
				Bone* parentBone = armature->GetBoneByName(trootBone->parentName);
				glm::mat4 temp_matrix = glm::inverse(parentBone->GetBindPoseMatrix()) * trootBone->GetBindPoseMatrix();
				translation_to_apply = total_translation - temp_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			} else {
				translation_to_apply = total_translation;
				// Also, since this bone has no parent in the armature heirarchy, it may be childed to another regular object which may be moving around
				// If so, we have to respect that
				Model* parentModel = getParentModelForBone(trootBone, scene);
				if (parentModel != nullptr) {
					if (parentModel->rigidAnimationDatas->size() != 0) {
						RigidAnimationData* parentRigidAnimationData = parentModel->rigidAnimationDatas->at(0);
						if (parentRigidAnimationData->hasKeyFrameAtTime(time)) {
							RigidAnimationKeyframe* parentRigidKeyframe = parentRigidAnimationData->GetExistingKeyframeAtTime(time);
							translation_to_apply += glm::vec4(parentRigidKeyframe->translation.x, parentRigidKeyframe->translation.y, parentRigidKeyframe->translation.z, 0.0f);
						}
					}
				}
			}
			if (translation_to_apply.x != 0.0f || translation_to_apply.y != 0.0f || translation_to_apply.z != 0.0f) {
				trootBone->Translate(glm::length(translation_to_apply),
									 glm::vec3(translation_to_apply.x, translation_to_apply.y, translation_to_apply.z), false);
			}
			// trootBone->Translate(translation.z, glm::vec3(0.0f, 0.0f, 1.0f), false);
			// trootBone->Translate(translation.y, glm::vec3(0.0f, 1.0f, 0.0f), false);
			// trootBone->Translate(translation.x, glm::vec3(1.0f, 0.0f, 0.0f), false);
#endif
			//
			glm::vec3 rotation = rigidKeyframe->rotation;
			//
			if (trootBone->parentName == "") {
				// Also, since this bone has no parent in the armature heirarchy, it may be childed to another regular object which may be moving around
				// If so, we have to respect that
				Model* parentModel = getParentModelForBone(trootBone, scene);
				if (parentModel->rigidAnimationDatas->size() != 0) {
					RigidAnimationData* parentRigidAnimationData = parentModel->rigidAnimationDatas->at(0);
					if (parentRigidAnimationData->hasKeyFrameAtTime(time)) {
						RigidAnimationKeyframe* parentRigidKeyframe = parentRigidAnimationData->GetExistingKeyframeAtTime(time);
						rotation = concatenateEulerRotations(rotation, parentRigidKeyframe->rotation);
					}
				}
			}
			//
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
