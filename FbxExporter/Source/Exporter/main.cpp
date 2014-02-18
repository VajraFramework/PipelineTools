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
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Parsers/ParseScene.h"
#include "Exporter/Utilities/FileUtilities/FileUtilities.h"
#include "Exporter/Writers/WriteScene.h"

#include "FbxSdk/include/fbxsdk.h"

#include <iostream>

#define SAMPLE_FILENAME "assassin.fbx"
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
// #define SAMPLE_FILENAME "imposter.fbx"
// #define SAMPLE_FILENAME "SD_GrassFloor_A.fbx"
// #define SAMPLE_FILENAME "SD_StoneFloor_A.fbx"
// #define SAMPLE_FILENAME "guard.fbx"
// #define SAMPLE_FILENAME "SD_Plane_01.fbx"

std::vector<std::string> GetFbxFileNamesForImport() {
	std::vector<std::string> fileNames = FileUtilities::GetFileNamesInFolder(IMPORT_PATH);
	std::vector<std::string> fbxFileNamesForImport;
	for (unsigned int i = 0; i < fileNames.size(); ++i) {
		if (fileNames[i].find(".fbx") != std::string::npos) {
			fbxFileNamesForImport.push_back(fileNames[i]);
		}
	}
	return fbxFileNamesForImport;
}


int main (int argc, char** argv) {

    FbxManager* fbxManager = NULL;
    FbxScene* lScene = NULL;

    // Prepare the FBX SDK.
    InitializeSdkObjects(fbxManager);

	std::vector<std::string> fileNamesForImport = GetFbxFileNamesForImport();
	bool EXPORT_ALL_TO_ONE_FOLDER = true;

	for (unsigned int i = 0; i < fileNamesForImport.size(); ++i) {

		std::string fbxFileName = fileNamesForImport[i];

		Scene* scene = ParseScene(fbxManager, IMPORT_PATH + fbxFileName);

		scene->DEBUG_ExportNamesOfAllObjectsInScene();

		if (!EXPORT_ALL_TO_ONE_FOLDER) {
			std::string exportFolderName = fbxFileName.substr(0, fbxFileName.find("."));
			FileUtilities::CreateFolder(EXPORT_PATH_BASE + exportFolderName);
			FileUtilities::CreateFolder(EXPORT_PATH_BASE + exportFolderName + "/" EXPORT_FOLDER_MODELS);
			FileUtilities::CreateFolder(EXPORT_PATH_BASE + exportFolderName + "/" EXPORT_FOLDER_ANIMCLIPS);
			ExportSceneToModelFiles(scene, EXPORT_PATH_BASE + exportFolderName + "/");

		} else {
			FileUtilities::CreateFolder(EXPORT_PATH_BASE);
			FileUtilities::CreateFolder(EXPORT_PATH_BASE EXPORT_FOLDER_MODELS);
			FileUtilities::CreateFolder(EXPORT_PATH_BASE EXPORT_FOLDER_ANIMCLIPS);
			ExportSceneToModelFiles(scene, EXPORT_PATH_BASE);
		}
	}

    //Destroy all objects created by the FBX SDK.
    DestroySdkObjects(fbxManager, true);

	std::cin.get();
    return 0;
}
