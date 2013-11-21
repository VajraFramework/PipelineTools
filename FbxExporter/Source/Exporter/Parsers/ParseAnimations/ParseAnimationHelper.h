#ifndef PARSE_ANIMATION_HELPER_H
#define PARSE_ANIMATION_HELPER_H

#include "Exporter/Common/Common.h"
#include "Exporter/Parsers/ParseAnimations/ParseAnimation.h"
#include "FbxSdk/include/fbxsdk.h"

#include "Libraries/glm/glm.hpp"

#include <string>
#include <vector>

void processAnimCurvesForNodeInAllAnimationLayers(FbxNode* fbxNode, std::vector<AnimationLayer*>& animationLayers);

#endif // PARSE_ANIMATION_HELPER_H
