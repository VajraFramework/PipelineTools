#ifndef WRITE_SCENE_H
#define WRITE_SCENE_H

#include <string>

#define MODEL_FILE_EXTENSION ".model"
#define ANIMCLIPS_FILE_EXTENSION ".animclips"
#define SKELETAL_ANIMATION_FILE_EXTENSION ".skeletalanimation"
#define ARMATURE_FILE_EXTENSION ".armature"

#define ANIMATION_TYPE_STRING_RIGID "RIGID"
#define ANIMATION_TYPE_STRING_BAKED_SKELETAL "BAKED_SKELETAL"
#define CLIPNAME_STRING "CLIPNAME"

#define TEXTURE_SHADER_NAME "txrshdr"
#define COLOR_SHADER_NAME "clrshdr"
#define BONE_COLOR_SHADER_NAME "bncshdr"
#define BONE_TEXTURE_SHADER_NAME "bntshdr"

// Forward Declarations:
class Scene;

void ExportSceneToModelFiles(Scene* scene, std::string basePath);

#endif // WRITE_SCENE_H
