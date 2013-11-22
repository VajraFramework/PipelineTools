#include "Exporter/Parsers/ParseAnimations/ParseAnimationHelper.h"
#include "Exporter/Parsers/ParseAnimations/ParseAnimation.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Utilities/Utilities.h"

// Forward Declarations:
void printKeyFrames(RigidAnimationData* rigidAnimationData);

FbxAnimCurve* getFbxAnimCurveForPropertyField(FbxNode* fbxNode, FbxAnimLayer* fbxAnimLayer, RigidAnimationProperty_t animation_property, RigidAnimationPropertyField_t field) {
	if (fbxAnimLayer == nullptr || fbxNode == nullptr) {
		return nullptr;
	}

	switch (animation_property) {
	case PROPERTY_TRANSLATION:
	{
		switch (field) {
		case PROPERTY_FIELD_X:
			return fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		case PROPERTY_FIELD_Y:
			return fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		case PROPERTY_FIELD_Z:
			return fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		}
	} break;

	case PROPERTY_ROTATION:
	{
		switch (field) {
		case PROPERTY_FIELD_X:
			return fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		case PROPERTY_FIELD_Y:
			return fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		case PROPERTY_FIELD_Z:
			return fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		}
	} break;

	case PROPERTY_SCALING:
	{
		switch (field) {
		case PROPERTY_FIELD_X:
			return fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		case PROPERTY_FIELD_Y:
			return fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		case PROPERTY_FIELD_Z:
			return fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		}
	} break;
	}

	return nullptr;
}

void processAnimCurveForPropertyField_internal(FbxAnimCurve* fbxAnimCurve, RigidAnimationProperty_t animation_property, RigidAnimationPropertyField_t field, RigidAnimationData* out_rigidAnimationData) {
	int numKeys = fbxAnimCurve->KeyGetCount();
	printf("\n_____________ Found anim curve with %d keys", numKeys);

	for (int keyIdx = 0; keyIdx < numKeys; ++keyIdx) {
		float keyValue = static_cast<float>(fbxAnimCurve->KeyGetValue(keyIdx));
        FbxTime keyTime  = fbxAnimCurve->KeyGetTime(keyIdx);

        out_rigidAnimationData->AddKeyframeForPropertyField(animation_property, field, keyValue, keyTime.GetSecondDouble());
	}
}

void processAnimCurveForPropertyFieldForNode(FbxNode* fbxNode, AnimationLayer* animationLayer, RigidAnimationProperty_t animation_property, RigidAnimationPropertyField_t field, RigidAnimationData* out_rigidAnimationData) {
	FbxAnimCurve* fbxAnimCurve = getFbxAnimCurveForPropertyField(fbxNode, animationLayer->fbxAnimLayer, animation_property, field);
	if (fbxAnimCurve != nullptr) {
		processAnimCurveForPropertyField_internal(fbxAnimCurve, animation_property, field, out_rigidAnimationData);
	}
}

void processAnimCurvesForPropertyForNode(FbxNode* fbxNode, AnimationLayer* animationLayer, RigidAnimationProperty_t animation_property, RigidAnimationData* out_rigidAnimationData) {
	processAnimCurveForPropertyFieldForNode(fbxNode, animationLayer, animation_property, PROPERTY_FIELD_X, out_rigidAnimationData);
	processAnimCurveForPropertyFieldForNode(fbxNode, animationLayer, animation_property, PROPERTY_FIELD_Y, out_rigidAnimationData);
	processAnimCurveForPropertyFieldForNode(fbxNode, animationLayer, animation_property, PROPERTY_FIELD_Z, out_rigidAnimationData);
}

void processAnimCurvesForNodeInAnimationLayer(FbxNode* fbxNode, AnimationLayer* animationLayer, RigidAnimationData* out_rigidAnimationData) {
	processAnimCurvesForPropertyForNode(fbxNode, animationLayer, PROPERTY_TRANSLATION, out_rigidAnimationData);
	processAnimCurvesForPropertyForNode(fbxNode, animationLayer, PROPERTY_ROTATION, out_rigidAnimationData);
	processAnimCurvesForPropertyForNode(fbxNode, animationLayer, PROPERTY_SCALING, out_rigidAnimationData);
}

std::vector<RigidAnimationData*>* processAnimCurvesForNodeInAllAnimationLayers(FbxNode* fbxNode, std::vector<AnimationLayer*>& animationLayers) {
	std::vector<RigidAnimationData*>* rigidAnimationDatas = new std::vector<RigidAnimationData*>();
	int layer_idx = 0;
	for (auto animationLayer_it = animationLayers.begin(); animationLayer_it != animationLayers.end(); ++animationLayer_it) {
		AnimationLayer* animationLayer = *animationLayer_it;

		RigidAnimationData* rigidAnimationData = new RigidAnimationData();
		char layer_idx_buffer[10];
		sprintf(layer_idx_buffer, "%d", layer_idx);
		rigidAnimationData->SetName(animationLayer->animationStackName + layer_idx_buffer);
		processAnimCurvesForNodeInAnimationLayer(fbxNode, animationLayer, rigidAnimationData);

		if (rigidAnimationData->GetNumKeyframes() > 1) {
			printKeyFrames(rigidAnimationData);
			rigidAnimationDatas->push_back(rigidAnimationData);
		} else {
			// Has only 1 keyframe in all. No need to consider it as an "animation"
			delete rigidAnimationData;
		}

		++layer_idx;
	}

	return rigidAnimationDatas;
}

void printKeyFrames(RigidAnimationData* rigidAnimationData) {
	printf("\nPrinting Keyframes: ");
	int numKeyFrames = rigidAnimationData->GetNumKeyframes();
	for (unsigned int i = 0; i < numKeyFrames; ++i) {
		RigidAnimationKeyframe* keyframe = rigidAnimationData->GetKeyframeAtIndex(i);
		printf("\nKeyframe %u:", i);
		printf("\n\ttime: %f", keyframe->time);
		printf("\n\ttranslation: "); printGlmVec3(keyframe->translation);
		printf("\n\trotation: "); printGlmVec3(keyframe->rotation);
		printf("\n\tscaling: "); printGlmVec3(keyframe->scaling);
	}
}
