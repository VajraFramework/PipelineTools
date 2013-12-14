#ifndef REPLAY_SKELETAL_ANIMATIONS_H
#define REPLAY_SKELETAL_ANIMATIONS_H

#include "FbxSdk/include/fbxsdk.h"

#include <string>

// Forward Declarations:
class Scene;

void ReplaySkeletalAnimationIntoSkeletalAnimationData(Armature* armature, Scene* scene, std::vector<float>& timesOfKeyframes);


#endif // REPLAY_SKELETAL_ANIMATIONS_H
