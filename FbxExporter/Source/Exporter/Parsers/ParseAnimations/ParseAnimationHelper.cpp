#include "Exporter/Parsers/ParseAnimations/ParseAnimationHelper.h"
#include "Exporter/Parsers/ParseAnimations/ParseAnimation.h"

void processAnimCurveForNode(FbxNode* fbxNode, FbxAnimCurve* fbxAnimCurve) {
	if (fbxAnimCurve == nullptr) {
		return;
	}
	int numKeys = fbxAnimCurve->KeyGetCount();
	printf("\n_____________ Found anim curve with %d keys", numKeys);
}

void processAnimCurvesForNodeInAnimationLayer(FbxNode* fbxNode, AnimationLayer* animationLayer) {
	FbxAnimCurve* fbxAnimCurve = fbxNode->LclTranslation.GetCurve(animationLayer->fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (fbxAnimCurve != nullptr) {
		processAnimCurveForNode(fbxNode, fbxAnimCurve);
	} else {
		printf("\n_____________ No anim curve");
	}
}

void processAnimCurvesForNodeInAllAnimationLayers(FbxNode* fbxNode, std::vector<AnimationLayer*>& animationLayers) {
	for (AnimationLayer* animationLayer : animationLayers) {
		processAnimCurvesForNodeInAnimationLayer(fbxNode, animationLayer);
	}
}
