#include "Exporter/Parsers/ParseBones/ParseBones.h"
#include "Exporter/Utilities/Utilities.h"

std::string cleanUpName(std::string name) {
	size_t colonPos = name.find(":");
	while (colonPos != std::string::npos) {
		name = name.replace(colonPos, 1, "_");
		colonPos = name.find(":");
	}
	return name;
}

////////////////////////////////////////////////////////////////////////////////

bool isNodeABone(FbxNode* fbxNode) {
	return (fbxNode->GetNodeAttribute() != nullptr &&
			fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);
}

void processWeights(Bone* bone, FbxCluster* fbxCluster) {
	int numIndices = fbxCluster->GetControlPointIndicesCount();
	int* indices = fbxCluster->GetControlPointIndices();
	double* weights = fbxCluster->GetControlPointWeights();

	printf("\nFound %d bone weight influences", numIndices);

	for (int idx = 0; idx < numIndices; ++idx) {
		BoneInfluence* influence = new BoneInfluence();
		influence->controlPointIdx = indices[idx];
		influence->weight = weights[idx];
		//
		bone->influences.push_back(influence);
	}
}

void processBoneNode(Armature* armature, Bone* bone, FbxCluster* fbxCluster) {

	FbxNode* boneNode = fbxCluster->GetLink();

	if (!isNodeABone(boneNode)) {
		printf("\nWARNING: Not a bone. (name = %s)", boneNode->GetName());
		return;
	}

	FbxSkeleton* fbxSkeleton = (FbxSkeleton*) boneNode->GetNodeAttribute();

	// Set bone's name:
	bone->SetName(boneNode->GetName());
	printf("\n\nProcessing bone: %s", bone->name.c_str());

	// Set bone's parent:
	if (isNodeABone(boneNode->GetParent())) {
		bone->SetParent(cleanUpName(boneNode->GetParent()->GetName()));
		printf("\nParent bone: %s", bone->parentName.c_str());
	}

	// Set bone's children:
	printf("\nChild bones:");
	int numChildren = boneNode->GetChildCount();
	for (int childIdx = 0; childIdx < numChildren; ++childIdx) {
		FbxNode* childBoneNode = boneNode->GetChild(childIdx);
		if (isNodeABone(childBoneNode)) {
			bone->AddChild(cleanUpName(childBoneNode->GetName()));
			printf("\n\t%s", cleanUpName(childBoneNode->GetName()).c_str());
		}
	}

	if (fbxSkeleton->IsSkeletonRoot()) {
		armature->SetRootBoneName(bone->name);
	}

	// Set bone's bind pose matrix:
	{
		FbxAMatrix bindPoseMatrix;
		bindPoseMatrix = fbxCluster->GetTransformLinkMatrix(bindPoseMatrix);
		glm::mat4x4 m = ConvertFbxMatrixToGlmMat4x4(bindPoseMatrix);
		printf("\nCluster link transform (bind pose) matrix:\n");
		printGlmMat4x4(m);
		printf("\n");
		//
		bone->bindPoseMatrix = m;
	}

	{
		// Process weights:
		processWeights(bone, fbxCluster);
	}

	armature->AddBone(bone);
}

/*
 * A deformer/skin is ~= an armature
 * A deformer/skin has many clusters
 * A cluster  is ~= a  bone
 */

Armature* processSkinDeformer(FbxDeformer* fbxDeformer, FbxGeometry* affectedGeometry) {
	Armature* armature = new Armature();
	armature->SetName(affectedGeometry->GetNode()->GetName());

	FbxSkin* fbxSkin = (FbxSkin*)fbxDeformer;

	int numClusters = fbxSkin->GetClusterCount();
	printf("\nNumber of clusters found: %d", numClusters);

	for (int clusterIdx = 0; clusterIdx < numClusters; ++clusterIdx) {
		FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIdx);
		ASSERT(fbxCluster->GetLink() != nullptr, "Link not null");

		Bone* bone = new Bone();
		bone->idx = clusterIdx;

		processBoneNode(armature, bone, fbxCluster);
	}

	printf("\nDone processing armature, %s.\nRoot bone name: %s\n", armature->name.c_str(), armature->GetRootBoneName().c_str());

	return armature;
}

Armature* ParseSkinClusters(FbxGeometry* fbxGeometry) {
	int numSkinDeformers = fbxGeometry->GetDeformerCount(FbxDeformer::eSkin);
	if (numSkinDeformers == 0) {
		return nullptr;
	}

	printf("\nNumber of skin deformers found: %d", numSkinDeformers);

	// TODO [Implement] Add support for multiple skin deformers acting on the same mesh:
	return processSkinDeformer(fbxGeometry->GetDeformer(0, FbxDeformer::eSkin), fbxGeometry);
}

////////////////////////////////////////////////////////////////////////////////

void AddBoneWeightsToMesh(Armature* armature, Mesh* mesh) {
	for (auto bone_it = armature->bones.begin(); bone_it != armature->bones.end(); ++bone_it) {
		Bone* bone = bone_it->second;

		for (auto influence_it = bone->influences.begin(); influence_it != bone->influences.end(); ++influence_it) {
			BoneInfluence* influence = *influence_it;

			mesh->AddBoneWeightInfluenceToVertexAtIndex(influence->controlPointIdx, bone->idx, influence->weight);
		}
	}

}
