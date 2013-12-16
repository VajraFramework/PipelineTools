#include "Exporter/Definitions/SkeletalAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Utilities/Utilities.h"

#include "Libraries/glm/gtc/matrix_inverse.hpp"
#include "Libraries/glm/gtc/quaternion.hpp"
#include "Libraries/glm/gtx/quaternion.hpp"
#include "Libraries/glm/gtx/transform.hpp"

static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0f);

Bone::Bone(Armature* armature_) {
	this->idx = -1;
	this->armature = armature_;
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

void Bone::SetBindPoseMatrix(glm::mat4 m) {
	this->bindPoseMatrix = m;

	/*
	 * Note: the bindPoseMatrix here is the matrix with which if we draw any "bone model",
	 * it would end up in the correct place visually
	 */

	this->toWorldMatrix = this->bindPoseMatrix;
	this->toBoneMatrix  = glm::inverse(this->toWorldMatrix);
}

void Bone::ResetLocalRotations() {
	this->localRotationMatrix = IDENTITY_MATRIX;
}

void Bone::Rotate(float angleInDegrees, glm::vec3 axis, bool boneSpace /*= false*/) {
	if (!boneSpace) {
		// Axis specified in object space coordinates
		// Convert it to boneSPace
		glm::vec4 newAxis = this->toBoneMatrix * glm::vec4(axis.x, axis.y, axis.z, 0.0f);
		axis = glm::vec3(newAxis.x, newAxis.y, newAxis.z);
	}

	if (axis != glm::vec3(0.0f, 0.0f, 0.0f)) {
		this->localRotationMatrix = this->localRotationMatrix * glm::rotate(angleInDegrees, axis);
	}
}

void Bone::init() {
	this->bindPoseMatrix  = IDENTITY_MATRIX;
	//
	this->toBoneMatrix    = IDENTITY_MATRIX;
	this->toWorldMatrix   = IDENTITY_MATRIX;
	//
	this->localRotationMatrix    = IDENTITY_MATRIX;
	this->localTranslationMatrix = IDENTITY_MATRIX;
	this->localMatrixCumulative  = IDENTITY_MATRIX;
}

void Bone::destroy() {

}

void Bone::updateBoneMatricesRecursive() {
	glm::mat4 matrixToPropogate = this->toWorldMatrix *
		this->localRotationMatrix * this->localTranslationMatrix *
		this->toBoneMatrix;
	//
	for (auto it = this->childrenNames.begin(); it != this->childrenNames.end(); ++it) {
		std::string childBoneName = *it;
		Bone* childBone = this->armature->GetBoneByName(childBoneName);
		childBone->propogateRawMatrixToChildren(matrixToPropogate);
	}

	glm::mat4 finalBoneTransformMatrix = this->localMatrixCumulative *
										 this->toWorldMatrix *
										 this->localRotationMatrix * this->localTranslationMatrix;

	this->armature->finalBoneTransforms[this->idx] = finalBoneTransformMatrix *
													 this->toBoneMatrix;

#if DRAW_BONES
	this->visualizer->GetTransform()->SetModelMatrixCumulative(((GameObject*)this->armature->GetObject())->GetTransform()->GetModelMatrixCumulative() *
		finalBoneTransformMatrix);
#endif


	// Do the same for children bones:
	for (auto it = this->childrenNames.begin(); it != this->childrenNames.end(); ++it) {
		std::string childBoneName = *it;
		Bone* childBone = this->armature->GetBoneByName(childBoneName);
		childBone->updateBoneMatricesRecursive();
	}

}

void Bone::propogateRawMatrixToChildren(glm::mat4 rawMatrix) {
	this->localMatrixCumulative = this->localMatrixCumulative * rawMatrix;

	for (auto it = this->childrenNames.begin(); it != this->childrenNames.end(); ++it) {
		std::string childBoneName = *it;
		Bone* childBone = this->armature->GetBoneByName(childBoneName);
		childBone->propogateRawMatrixToChildren(rawMatrix);
	}
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

void Armature::UpdateFinalBoneMatrices() {
	for (auto bone_it = this->bones.begin(); bone_it != this->bones.end(); ++bone_it) {
		Bone* bone = bone_it->second;
		bone->localMatrixCumulative = IDENTITY_MATRIX;
	}

	Bone* rootBone = this->GetBoneByName(this->GetRootBoneName());
	ASSERT(rootBone != nullptr, "Found root bone");

	rootBone->updateBoneMatricesRecursive();
}
