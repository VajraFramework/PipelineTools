/****************************************************************************************

   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Parsers/ParseScene.h"
#include "Exporter/Writers/WriteScene.h"

#include "FbxSdk/include/fbxsdk.h"

#define SAMPLE_FILENAME "Resources/Import/Assassin.fbx"
// #define SAMPLE_FILENAME "Resources/Import/flyer.fbx"
// #define SAMPLE_FILENAME "Resources/Import/monkey.fbx"
// #define SAMPLE_FILENAME "Resources/Import/TexturedCube.fbx"
// #define SAMPLE_FILENAME "Resources/Import/twoplanes.fbx"
// #define SAMPLE_FILENAME "Resources/Import/SimpleCube.fbx"

#define EXPORT_PATH "Resources/Export/Models/"

//get mesh normals info
void GetNormals(FbxNode* pNode);

int main (int argc, char** argv) {

    FbxManager* fbxManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;

    // Prepare the FBX SDK.
    InitializeSdkObjects(fbxManager);

	Scene* scene = ParseScene(fbxManager, SAMPLE_FILENAME);

	ExportSceneToModelFiles(scene, EXPORT_PATH);

    //Destroy all objects created by the FBX SDK.
    DestroySdkObjects(fbxManager, true);
    return 0;
}
