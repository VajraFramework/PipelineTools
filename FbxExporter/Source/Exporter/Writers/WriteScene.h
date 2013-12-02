#ifndef WRITE_SCENE_H
#define WRITE_SCENE_H

#include <string>

#define MODEL_FILE_EXTENSION ".model"
#define ANIMCLIPS_FILE_EXTENSION ".animclips"
#define ARMATURE_FILE_EXTENSION ".armature"

#define ANIMATION_TYPE_STRING_RIGID "RIGID"
#define CLIPNAME_STRING "CLIPNAME"

#define TEXTURE_SHADER_NAME "txrshdr"
#define COLOR_SHADER_NAME "clrshdr"
#define BONE_COLOR_SHADER_NAME "bncshdr"

// Forward Declarations:
class Scene;

void ExportSceneToModelFiles(Scene* scene, std::string basePath);

#endif // WRITE_SCENE_H
