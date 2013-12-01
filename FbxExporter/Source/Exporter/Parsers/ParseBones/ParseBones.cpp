#include "Exporter/Parsers/ParseBones/ParseBones.h"
#include "Exporter/Utilities/Utilities.h"

#if 0

FbxPose* findBindPose(FbxScene* fbxScene) {
	int numPoses = fbxScene->GetPoseCount();
	printf("\nNumber of poses: %d", numPoses);

	int poseIdx = 0;
	for (; poseIdx < numPoses; ++poseIdx) {
		FbxPose* pose = fbxScene->GetPose(poseIdx);
		if (pose->IsBindPose()) {
			return fbxScene->GetPose(poseIdx);
		}
	}

	return nullptr;
}

int numBonesInBindPose(FbxPose* fbxPose) {
	int numBones = 0;

	int numItemsInPose = fbxPose->GetCount();
	for (int poseItemIdx = 0; poseItemIdx < numItemsInPose; ++poseItemIdx) {
		FbxNode* fbxNodeInPose = fbxPose->GetNode(poseItemIdx);
		if (fbxNodeInPose->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
			numBones++;
		}
	}

	return numBones;
}

Armature* processBonesInBindPose(FbxPose* fbxPose) {
	int numItemsInPose = fbxPose->GetCount();
	printf("\nNumber of items in pose: %d\n", numItemsInPose);

	if (numBonesInBindPose(fbxPose) == 0) {
		return nullptr;
	}

	Armature* armature = new Armature();

	for (int poseItemIdx = 0; poseItemIdx < numItemsInPose; ++poseItemIdx) {
		FbxNode* fbxNodeInPose = fbxPose->GetNode(poseItemIdx);
		if (fbxNodeInPose->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
			FbxNode* boneNode = fbxNodeInPose;
			Bone* bone = new Bone();

			bone->name = boneNode->GetName();
			printf("\nFound bone : %s", bone->name.c_str());
			//
			printf("\nBind pose matrix for: %s", fbxPose->GetNode(poseItemIdx)->GetName());
			//
			ASSERT(fbxPose->IsLocalMatrix(poseItemIdx), "Is local matrix (non-local matrices in bind pose are not yet supported)");
			printf("\nIs local space? %s\n", fbxPose->IsLocalMatrix(poseItemIdx) ? "true" : "false");
			//
			FbxMatrix fbxMatrix = fbxPose->GetMatrix(poseItemIdx);
			bone->bindPoseMatrix = ConvertFbxMatrixToGlmMat4x4(fbxMatrix);
			printGlmMat4x4(bone->bindPoseMatrix);
			//
			FbxSkeleton* fbxSkeleton = (FbxSkeleton*) boneNode->GetNodeAttribute();
			switch(fbxSkeleton->GetSkeletonType()) {
			case FbxSkeleton::eLimb: printf("Skeleton node type: limb"); break;
			case FbxSkeleton::eLimbNode: printf("Skeleton node type: limb node"); break;
			case FbxSkeleton::eRoot: printf("Skeleton node type: root"); break;
			case FbxSkeleton::eEffector: printf("Skeleton node type: effector"); break;
			default: printf("Skeleton node type: other"); break;
			}
			//
			printf("\nIs root bone?: %s", fbxSkeleton->IsSkeletonRoot() ? "true" : "false");
			//
			printf("\nNumber of children: %d", boneNode->GetChildCount());
			//
			printf("\n");
		}
	}

	return armature;
}


Armature* processBindPose(FbxPose* fbxPose) {

	return processBonesInBindPose(fbxPose);
}

Armature* ParseBones(FbxScene* fbxScene) {
	FbxPose* bindPose = findBindPose(fbxScene);

	if (bindPose == nullptr) {
		printf("\nNo bind pose found");
		return nullptr;
	}

	printf("\nBind pose found: %s", bindPose->GetName());

	Armature* armature = processBindPose(bindPose);

	return armature;
}

#endif

////////////////////////////////////////////////////////////////////////////////

bool isNodeABone(FbxNode* fbxNode) {
	return (fbxNode->GetNodeAttribute() != nullptr &&
			fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);
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
	printf("\nProcessing bone: %s", bone->name.c_str());

	// Set bone's parent:
	if (isNodeABone(boneNode->GetParent())) {
		bone->SetParent(boneNode->GetParent()->GetName());
		printf("\nParent bone: %s", bone->parentName.c_str());
	}

	// Set bone's children:
	printf("\nChild bones:");
	int numChildren = boneNode->GetChildCount();
	for (int childIdx = 0; childIdx < numChildren; ++childIdx) {
		FbxNode* childBoneNode = boneNode->GetChild(childIdx);
		if (isNodeABone(childBoneNode)) {
			bone->AddChild(childBoneNode->GetName());
			printf("\n\t%s", childBoneNode->GetName());
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
