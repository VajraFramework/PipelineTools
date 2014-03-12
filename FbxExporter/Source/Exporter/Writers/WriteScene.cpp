#include "Exporter/Common/Declarations.h"
#include "Exporter/Definitions/AnimClipKeyframes.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Scene.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "Exporter/Utilities/FileUtilities/FileUtilities.h"
#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Writers/WriteScene.h"
#include "Exporter/Writers/WriteArmature.h"
#include "Exporter/Writers/WriteBakedSkeletalAnimation.h"

#include <fstream>
#include <vector>

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
		// Write out positions, normals, texCoords, and bone weight influences
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
		if (mesh->textureFileName != "") {
			WriteVectorOfVec2ToFile(texCoords, file);
		} else {
			file << "0" << "\n";
		}

		// Write out bone weight influences:
		if (mesh->armature != nullptr) {
			file << numVertices << "\n";
			for (int vidx = 0; vidx < numVertices; ++vidx) {
				Vertex* vertex = mesh->vertices->at(vidx);
				for (unsigned int i = 0; i < vertex->influences.size(); ++i) {
					file << vertex->influences[i]->boneIdx << " " << vertex->influences[i]->weight << " ";
				}
				file << "\n";
			}
		} else {
			file << "0" << "\n";
		}
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
		// Write out material data:
		WriteGlmVec3ToFile(mesh->material->ambientColor, file);    file << "\n";
		WriteGlmVec3ToFile(mesh->material->diffuseColor, file);    file << "\n";
		WriteGlmVec3ToFile(mesh->material->specularColor, file);    file << "\n";
	}

	{
		// Write out texture info:

		// TODO [Implement] Multiple textures
		if (mesh->textureFileName != "") {
			file << "yes" << "\n";
			file << mesh->textureFileName << "\n";
		} else {
			file << "no" << "\n";
		}
	}

	{
		// Write out armature name:
		if (mesh->armature != nullptr) {
			file << "yes" << "\n";
			file << mesh->armature->name << ARMATURE_FILE_EXTENSION << "\n";
		} else {
			file << "no" << "\n";
		}
	}

	{
		// Write out shader name:
		if (mesh->textureFileName == "") {
			if (mesh->armature != nullptr) {
				file << BONE_COLOR_SHADER_NAME << "\n";
			} else {
				file << COLOR_SHADER_NAME << "\n";
			}
		} else {
			if (mesh->armature != nullptr) {
				file << BONE_TEXTURE_SHADER_NAME << "\n";
			} else {
				file << TEXTURE_SHADER_NAME << "\n";
			}
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

void exportRigidAnimationKeyframe(Model* model, RigidAnimationKeyframe* keyframe, std::ofstream& file) {

	if (!file.good()) {
		VERIFY(0, "Failed to open animclips file for model %s", model->name.c_str());
	}

	// int numKeyframes = rigidAnimationData->GetNumKeyframes();
	// file << numKeyframes << "\n";
	// for (int i = 0; i < numKeyframes; ++i) {
		// RigidAnimationKeyframe* keyframe = rigidAnimationData->GetKeyframeAtIndex(i);

		file << keyframe->time << " ";
		WriteGlmVec3ToFile(keyframe->translation, file);  file << " ";
		WriteGlmVec3ToFile(keyframe->rotation, file);  file << " ";
		WriteGlmVec3ToFile(keyframe->scaling, file);  file << " ";

		file << "\n";
	// }
}

void exportRigidAnimations(Model* model, std::ofstream& file) {
	printf("\nExporting animclips for model: %s", model->name.c_str());

	if (!file.good()) {
		VERIFY(0, "Failed to open animclips file for model %s", model->name.c_str());
	}

	file << ANIMATION_TYPE_STRING_RIGID << "\n\n";

	// Get the names of the animation clips and their respective keyframes:
	std::string animclipKeyframesFileName = model->name;
	animclipKeyframesFileName += ".animclipframes";
	std::vector<AnimClipKeyframes*> vectorOfAnimclipKeyframes;
	if (FileUtilities::FileExistsInFolder(IMPORT_PATH, animclipKeyframesFileName)) {
		vectorOfAnimclipKeyframes = extractAnimclipKeyframeNumbersFromFile(IMPORT_PATH + animclipKeyframesFileName);
	}

	int numRigidAnimations = vectorOfAnimclipKeyframes.size();
	file << numRigidAnimations << "\n\n";

	// TODO [Hack]
	RigidAnimationData* rigidAnimationData = model->rigidAnimationDatas->at(0);

	for (unsigned int i = 0; i < vectorOfAnimclipKeyframes.size(); ++i) {
		file << CLIPNAME_STRING << "#" << vectorOfAnimclipKeyframes[i]->clipName << "\n";

		int numKeyframes = vectorOfAnimclipKeyframes[i]->endKeyframeNumber - vectorOfAnimclipKeyframes[i]->startKeyframeNumber + 1;
		file << numKeyframes << "\n\n";

		ASSERT(vectorOfAnimclipKeyframes[i]->startKeyframeNumber <= skeletalAnimationData->GetNumKeyframes(), "Valid start keyframe number: %d", vectorOfAnimclipKeyframes[i]->startKeyframeNumber);
		ASSERT(vectorOfAnimclipKeyframes[i]->endKeyframeNumber <= skeletalAnimationData->GetNumKeyframes(), "Valid end keyframe number: %d", vectorOfAnimclipKeyframes[i]->endKeyframeNumber);
		
		for (int frameNumber = vectorOfAnimclipKeyframes[i]->startKeyframeNumber; frameNumber <= vectorOfAnimclipKeyframes[i]->endKeyframeNumber; ++frameNumber) {
			// Note that frameNumbers start from 1 instead of 0, adjust accordingly:
			int frameNumberIdx = frameNumber - 1;
			//
			exportRigidAnimationKeyframe(model, rigidAnimationData->GetKeyframeAtIndex(frameNumberIdx), file);
		}

		file << "\n";
	}
}

void exportScene(Scene* scene, std::string basePath) {
	std::string modelsPath = basePath + EXPORT_FOLDER_MODELS;
	std::string animclipsPath = basePath + EXPORT_FOLDER_ANIMCLIPS;

	int numModelsInScene = scene->models->size();
	printf("\nNumber of models in scene: %d", numModelsInScene);

	for (int i = 0; i < numModelsInScene; ++i) {

		Model* model = scene->models->at(i);


		// Open new model file for writing:
		if (model->mesh != nullptr) {
			std::string modelFilePath = modelsPath + model->name + MODEL_FILE_EXTENSION;
			std::ofstream modelFile(modelFilePath, std::ios_base::out);
			//
			modelFile << MODEL_FORMAT_VERSION_NUMBER << "\n";
			//
			exportModel(model, modelFile);
			//
			modelFile.close();
		}


		// Open a new animclips file for writing:
		if (model->rigidAnimationDatas->size() != 0) {
			std::string animclipsFilePath = animclipsPath +  model->name + ANIMCLIPS_FILE_EXTENSION;
			std::ofstream animclipsFile(animclipsFilePath, std::ios_base::out);
			//
			animclipsFile << RIGID_ANIMATION_FORMAT_VERSION_NUMBER  << "\n";
			//
			exportRigidAnimations(model, animclipsFile);
			//
			animclipsFile.close();
		}


		// Open a new armature file for writing:
		if (model->mesh != nullptr && model->mesh->armature != nullptr) {
			std::string armatureFilePath = modelsPath + model->mesh->armature->name + ARMATURE_FILE_EXTENSION;
			std::ofstream armatureFile(armatureFilePath, std::ios_base::out);
			//
			armatureFile << ARMATURE_FORMAT_VERSION_NUMBER  << "\n";
			//
			exportArmature(model->mesh->armature, armatureFile);
			//
			armatureFile.close();
		}

		// Open a new baked skeletal animation file for writing:
		if (model->mesh != nullptr && model->mesh->armature != nullptr && model->mesh->armature->skeletalAnimationData != nullptr) {
			std::string skeletalAnimationFilePath = modelsPath + model->mesh->armature->name + SKELETAL_ANIMATION_FILE_EXTENSION;
			std::ofstream skeletalAnimationFile(skeletalAnimationFilePath, std::ios_base::out);
			//
			skeletalAnimationFile << SKELETAL_ANIMATION_FORMAT_VERSION_NUMBER  << "\n";
			//
			exportBakedSkeletalAnimation(model->mesh->armature, skeletalAnimationFile);
			//
			skeletalAnimationFile.close();
		}
	}

}

void ExportSceneToModelFiles(Scene* scene, std::string basePath) {
	printf("\nExporting scene %s, to path %s", scene->name.c_str(), basePath.c_str());

	exportScene(scene, basePath);
}

////////////////////////////////////////////////////////////////////////////////


