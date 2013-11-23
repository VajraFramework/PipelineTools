#include "Exporter/Common/Declarations.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Writers/WriteScene.h"

#include <fstream>
#include <vector>

#define MODEL_FILE_EXTENSION ".model"
#define ANIMCLIPS_FILE_EXTENSION ".animclips"

#define ANIMATION_TYPE_STRING_RIGID "RIGID"
#define CLIPNAME_STRING "CLIPNAME"

#define TEXTURE_SHADER_NAME "smplshdr"
#define COLOR_SHADER_NAME "clrshdr"

// Forward Declarations:
void WriteGlmVec3ToFile(glm::vec3 v, std::ofstream& file);
void WriteGlmVec2ToFile(glm::vec2 v, std::ofstream& file);
void WriteVectorOfVec3ToFile(std::vector<glm::vec3>* vectorOfVec3, std::ofstream& file);
void WriteVectorOfVec2ToFile(std::vector<glm::vec2>* vectorOfVec2, std::ofstream& file);

void exportMesh(Mesh* mesh, std::ofstream& file) {
	if (mesh == nullptr) {
		return;
	}

	printf("\nExporting mesh: %s", mesh->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open model file for mesh %s", mesh->name.c_str());
	}

	file << mesh->name << "\n";

	{
		// Write out positions, normals, and texCoords
		std::vector<glm::vec3>* positions = new std::vector<glm::vec3>();
		std::vector<glm::vec3>* normals = new std::vector<glm::vec3>();
		std::vector<glm::vec2>* texCoords = new std::vector<glm::vec2>();

		int numVertices = mesh->vertices->size();

		for (int vidx = 0; vidx < numVertices; ++vidx) {
			Vertex* vertex = mesh->vertices->at(vidx);

			positions->push_back(vertex->position);
			normals->push_back(vertex->normal);
			texCoords->push_back(vertex->texCoords);
		}

		printf("\nNumber of positions: %lu", positions->size());
		printf("\nNumber of normals: %lu", normals->size());
		printf("\nNumber of texCoords: %lu", texCoords->size());
		//
		WriteVectorOfVec3ToFile(positions, file);
		WriteVectorOfVec3ToFile(normals, file);
		WriteVectorOfVec2ToFile(texCoords, file);
	}

	{
		// Write out faces:

		// TODO [Implement] Multiple polylists:
		Polylist* polylist = mesh->polylists->at(0);
		int numFaces = polylist->faces->size();
		printf("\nNumber of faces: %d", numFaces);

		file << numFaces << "\n";

		for (int i = 0; i < numFaces; ++i) {
			Face* face = polylist->faces->at(i);
			file << face->indices[0] << " " << face->indices[1] << " " << face->indices[2];
			file << "\n";
		}
	}

	{
		// Write out texture info:

		// TODO [Implement] Multiple textures
		file << mesh->textureFileName << "\n";
	}

	{
		// Write out shader name:
		if (mesh->textureFileName == "") {
			file << COLOR_SHADER_NAME << "\n";
		} else {
			file << TEXTURE_SHADER_NAME << "\n";
		}
	}

	printf("\nDone exporting mesh: %s", mesh->name.c_str());
}

void exportModel(Model* model, std::ofstream& file) {
	printf("\nExporting model: %s", model->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open model file for model %s", model->name.c_str());
	}

	file << model->name << "\n";

	file << model->initialPosition.x << " " << model->initialPosition.y << " " << model->initialPosition.z << "\n";
	file << model->initialRotation.x << " " << model->initialRotation.y << " " << model->initialRotation.z << "\n";
	file << model->initialScale.x << " " << model->initialScale.y << " " << model->initialScale.z << "\n";

	exportMesh(model->mesh, file);

	printf("\nDone exporting model: %s", model->name.c_str());
	printf("\n");
}

void exportRigidAnimation(Model* model, RigidAnimationData* rigidAnimationData, std::ofstream& file) {
	printf("\nExporting rigidAnimationData for clip: %s", rigidAnimationData->GetName().c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open animclips file for model %s", model->name.c_str());
	}

	file << CLIPNAME_STRING << "#" << rigidAnimationData->GetName() << "\n";

	int numKeyframes = rigidAnimationData->GetNumKeyframes();
	file << numKeyframes << "\n";
	for (int i = 0; i < numKeyframes; ++i) {
		RigidAnimationKeyframe* keyframe = rigidAnimationData->GetKeyframeAtIndex(i);

		file << keyframe->time << " ";
		WriteGlmVec3ToFile(keyframe->translation, file);  file << " ";
		WriteGlmVec3ToFile(keyframe->rotation, file);  file << " ";
		WriteGlmVec3ToFile(keyframe->scaling, file);  file << " ";

		file << "\n";
	}
}

void exportRigidAnimations(Model* model, std::ofstream& file) {
	printf("\nExporting animclips for model: %s", model->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open animclips file for model %s", model->name.c_str());
	}

	file << ANIMATION_TYPE_STRING_RIGID << "\n\n";

	int numRigidAnimations = model->rigidAnimationDatas->size();
	file << numRigidAnimations << "\n\n";

	for (int i = 0; i < numRigidAnimations; ++i) {
		RigidAnimationData* rigidAnimationData = model->rigidAnimationDatas->at(i);
		exportRigidAnimation(model, rigidAnimationData, file);
	}

}

void exportScene(Scene* scene, std::string basePath) {
	int numModelsInScene = scene->models->size();
	printf("\nNumber of models in scene: %d", numModelsInScene);

	for (int i = 0; i < numModelsInScene; ++i) {

		Model* model = scene->models->at(i);

		// Open new model file for writing:
		std::string modelFilePath = basePath + model->name + MODEL_FILE_EXTENSION;
		std::ofstream modelFile(modelFilePath, std::ios_base::out);
		//
		modelFile << MODEL_FORMAT_VERSION_NUMBER << "\n";
		//
		exportModel(model, modelFile);
		//
		modelFile.close();

		// Open a new animclips file for writing:
		std::string animclipsFilePath = basePath + model->name + ANIMCLIPS_FILE_EXTENSION;
		std::ofstream animclipsFile(animclipsFilePath, std::ios_base::out);
		//
		animclipsFile << ANIMATION_FORMAT_VERSION_NUMBER  << "\n";
		//
		exportRigidAnimations(model, animclipsFile);
		//
		animclipsFile.close();
	}

}

void ExportSceneToModelFiles(Scene* scene, std::string basePath) {
	printf("\nExporting scene %s, to path %s", scene->name.c_str(), basePath.c_str());

	exportScene(scene, basePath);
}

////////////////////////////////////////////////////////////////////////////////

void WriteGlmVec3ToFile(glm::vec3 v, std::ofstream& file) {
	file << v.x << " " << v.y << " " << v.z;
}

void WriteGlmVec2ToFile(glm::vec2 v, std::ofstream& file) {
	file << v.x << " " << v.y;
}

void WriteVectorOfVec3ToFile(std::vector<glm::vec3>* vectorOfVec3, std::ofstream& file) {
	file << vectorOfVec3->size() << "\n";
	for (int i = 0; i < vectorOfVec3->size(); ++i) {
		WriteGlmVec3ToFile(vectorOfVec3->at(i), file);
		file << "\n";
	}
	file << "\n";
}

void WriteVectorOfVec2ToFile(std::vector<glm::vec2>* vectorOfVec2, std::ofstream& file) {
	file << vectorOfVec2->size() << "\n";
	for (int i = 0; i < vectorOfVec2->size(); ++i) {
		WriteGlmVec2ToFile(vectorOfVec2->at(i), file);
		file << "\n";
	}
	file << "\n";
}
