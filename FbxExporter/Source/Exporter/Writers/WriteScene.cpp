#include "Exporter/Common/Declarations.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Writers/WriteScene.h"

#include <fstream>
#include <vector>

#define MODEL_FILE_EXTENSION ".model"

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
		file << mesh->textureFileName;
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

void exportScene(Scene* scene, std::string basePath) {
	int numModelsInScene = scene->models->size();
	printf("\nNumber of models in scene: %d", numModelsInScene);

	for (int i = 0; i < numModelsInScene; ++i) {

		Model* model = scene->models->at(i);

		std::string modelFilePath = basePath + model->name + MODEL_FILE_EXTENSION;
		std::ofstream modelFile(modelFilePath, std::ios_base::out);

		modelFile << MODEL_FORMAT_VERSION_NUMBER << "\n";

		exportModel(model, modelFile);

		modelFile.close();
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
