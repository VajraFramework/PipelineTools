#include "Exporter/Parsers/ParseAnimations/ParseAnimation.h"
#include "Exporter/Parsers/ParseAnimations/ParseAnimationHelper.h"

std::vector<AnimationLayer*> gAnimationLayers;
std::vector<AnimationLayer*>& GetGlobalAnimationLayers() {
	return gAnimationLayers;
}

void LoadAnimationLayers(FbxScene* fbxScene) {
	gAnimationLayers.clear();

	printf("\nParsing scene for animations");

	int numStacks = fbxScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack));
	printf("\nNumber of animation stacks found: %d", numStacks);

	for (int stackIdx = 0; stackIdx < numStacks; ++stackIdx) {
		FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(stackIdx);
		std::string animationStackName = fbxAnimStack->GetName();
		printf("\nAnimation Stack name: %s", fbxAnimStack->GetName());

		int numAnimLayers = fbxAnimStack->GetMemberCount<FbxAnimLayer>();
		printf("\nNumber of animation layers found: %d", numAnimLayers);

		for (int animLayerIdx = 0; animLayerIdx < numAnimLayers; ++animLayerIdx) {
			FbxAnimLayer* fbxAnimLayer = fbxAnimStack->GetMember<FbxAnimLayer>(animLayerIdx);

			AnimationLayer* newAnimLayer = new AnimationLayer();
			newAnimLayer->animationStackName = animationStackName;
			newAnimLayer->fbxAnimLayer = fbxAnimLayer;
			gAnimationLayers.push_back(newAnimLayer);

			// FbxAnimCurve* lAnimCurve = lNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		}
	}

	printf("\nDone parsing rigid animations\n");
}

void ProcessAnimCurvesForNode(FbxNode* fbxNode) {
	processAnimCurvesForNodeInAllAnimationLayers(fbxNode, GetGlobalAnimationLayers());
}
