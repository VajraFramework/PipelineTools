#ifndef PARSE_SCENE_H
#define PARSE_SCENE_H

#include "FbxSdk/include/fbxsdk.h"

#include <string>

// Forward Declarations:
class Scene;

Scene* ParseScene(FbxManager*& fbxManager, std::string fbxFileName);

#endif // PARSE_SCENE_H
