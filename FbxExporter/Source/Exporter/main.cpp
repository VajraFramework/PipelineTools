/****************************************************************************************

   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Declarations.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Parsers/ParseScene.h"
#include "Exporter/Writers/WriteScene.h"

#include "FbxSdk/include/fbxsdk.h"

#include <iostream>

// #define SAMPLE_FILENAME "Assassin.fbx"
// #define SAMPLE_FILENAME "baserig.fbx"
// #define SAMPLE_FILENAME "flyer.fbx"
// #define SAMPLE_FILENAME "monkey.fbx"
// #define SAMPLE_FILENAME "TexturedCube.fbx"
// #define SAMPLE_FILENAME "texturedCubeBlender.fbx"
// #define SAMPLE_FILENAME "texturedCubeMaya.fbx"
// #define SAMPLE_FILENAME "pyramid.fbx"
// #define SAMPLE_FILENAME "twoplanes.fbx"
// #define SAMPLE_FILENAME "SimpleCube.fbx"
// #define SAMPLE_FILENAME "stubbyarrows.fbx"
// #define SAMPLE_FILENAME "wavybox.fbx"
// #define SAMPLE_FILENAME "bone.fbx"
// #define SAMPLE_FILENAME "thief.fbx"
// #define SAMPLE_FILENAME "guardcaptain.fbx"
// #define SAMPLE_FILENAME "newthief.fbx"
// #define SAMPLE_FILENAME "SD_Cube.fbx"
#define SAMPLE_FILENAME "imposter.fbx"

int main (int argc, char** argv) {

    FbxManager* fbxManager = NULL;
    FbxScene* lScene = NULL;

    // Prepare the FBX SDK.
    InitializeSdkObjects(fbxManager);

	Scene* scene = ParseScene(fbxManager, IMPORT_PATH SAMPLE_FILENAME);

	ExportSceneToModelFiles(scene, EXPORT_PATH);

    //Destroy all objects created by the FBX SDK.
    DestroySdkObjects(fbxManager, true);

	std::cin.get();
    return 0;
}
