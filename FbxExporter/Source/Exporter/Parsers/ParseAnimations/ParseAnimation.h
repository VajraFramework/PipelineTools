#ifndef PARSE_ANIMATION_H
#define PARSE_ANIMATION_H

#include "Exporter/Common/Common.h"
#include "FbxSdk/include/fbxsdk.h"

#include "Libraries/glm/glm.hpp"

#include <string>
#include <vector>

// Forward Declarations:
class RigidAnimationData;

struct AnimationLayer {
public:
	FbxAnimLayer* fbxAnimLayer;
	std::string animationStackName;
private:
};

std::vector<AnimationLayer*>& GetGlobalAnimationLayers();

void LoadAnimationLayers(FbxScene* fbxScene);
std::vector<RigidAnimationData*>* ProcessAnimCurvesForNode(FbxNode* fbxNode);

#endif // PARSE_ANIMATION_H
