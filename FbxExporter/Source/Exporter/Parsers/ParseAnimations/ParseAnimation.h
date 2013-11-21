#ifndef PARSE_ANIMATION_H
#define PARSE_ANIMATION_H

#include "Exporter/Common/Common.h"
#include "FbxSdk/include/fbxsdk.h"

#include "Libraries/glm/glm.hpp"

#include <string>
#include <vector>

struct AnimationLayer {
public:
	FbxAnimLayer* fbxAnimLayer;
	std::string animationStackName;
private:
};

std::vector<AnimationLayer*>& GetGlobalAnimationLayers();

void LoadAnimationLayers(FbxScene* fbxScene);
void ProcessAnimCurvesForNode(FbxNode* fbxNode);

#endif // PARSE_ANIMATION_H
