#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Definitions/Vertex.h"
#include "Exporter/Parsers/ParseAnimations/ParseAnimation.h"
#include "Exporter/Parsers/ParseScene.h"
#include "Exporter/Parsers/ParseMeshHelper.h"
#include "Exporter/Utilities/Utilities.h"

#include <fstream>
#include <vector>

// Forward Declarations:
void ParseNodesRecursively(FbxNode* trootNode, Scene* outScene);
void ParseNode(FbxNode* node, Scene* outScene);
//
Model* GetModel(FbxNode* node);
Mesh* GetMesh(FbxNode* node);

Scene* ParseScene(FbxManager*& fbxManager, std::string fbxFileName) {

	// Check if file exists:
	{
		std::ifstream fbxFile;
		fbxFile.open(fbxFileName, std::ios::in);
		VERIFY_LOG(fbxFile.is_open(), "File exists and is readable\n");
		fbxFile.close();
	}

	FbxScene* fbxScene = nullptr;
	fbxScene = FbxScene::Create(fbxManager, "NewScene");
	ASSERT_LOG(fbxScene != nullptr, "\nSuccessfully created new FBX fbxScene");

	Scene* scene = nullptr;

	{
		bool sceneLoadedSuccessfully = false;
		sceneLoadedSuccessfully = LoadScene(fbxManager, fbxScene, fbxFileName.c_str());
		ASSERT_LOG(sceneLoadedSuccessfully, "Scene from file %s loaded successfully", fbxFileName.c_str());

		// Look for Animation Layers:
		LoadAnimationLayers(fbxScene);

		scene = new Scene();
		scene->name = fbxFileName;
	}

	{
		FbxNode* rootNode = fbxScene->GetRootNode();
		ASSERT_LOG(rootNode != nullptr, "\nGot root node of the fbxScene");

		ParseNodesRecursively(rootNode, scene);
	}


	return scene;
}

void ParseNodesRecursively(FbxNode* trootNode, Scene* outScene) {
	ParseNode(trootNode, outScene);

	int numChildren = trootNode->GetChildCount();
	for (int i = 0; i < numChildren; ++i) {
		ParseNodesRecursively(trootNode->GetChild(i), outScene);
	}
}

void ParseNode(FbxNode* node, Scene* outScene) {
	ASSERT(node != nullptr, "node not null");
	printf("\nCurrent node: %s", node->GetName());

	Model* model = GetModel(node);
	if (model != nullptr) {
		outScene->models->push_back(model);
	}

	ProcessAnimCurvesForNode(node);

}

Model* GetModel(FbxNode* node) {
	Model* model = new Model();
	model->name = node->GetName();

	FbxVector4 initialPositionFbxVec4 = node->EvaluateLocalTranslation();
	model->initialPosition = ConvertFbxVector4ToGlmVec3(initialPositionFbxVec4);
	//
	FbxVector4 initialRotationFbxVec4 = node->EvaluateLocalRotation();
	model->initialRotation = ConvertFbxVector4ToGlmVec3(initialRotationFbxVec4);
	//
	FbxVector4 initialScaleFbxVec4 = node->EvaluateLocalScaling();
	model->initialScale = ConvertFbxVector4ToGlmVec3(initialScaleFbxVec4);

	Mesh* mesh = GetMesh(node);
	if (mesh != nullptr) {
		model->mesh = mesh;
	}

	return model;
}

Mesh* GetMesh(FbxNode* node) {

	Mesh* mesh = nullptr;

    FbxMesh* fbxMesh = node->GetMesh();
    if (fbxMesh) {
    	mesh = ParseFbxMesh(fbxMesh);

    	if (mesh->name == "") {
    		mesh->name = node->GetName();
    	}

    	PrintMesh(mesh);

    }

    return mesh;
}
