#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"
#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"


Model::Model() {
	this->mesh = nullptr;
	this->rigidAnimationDatas = nullptr;
}

Model::~Model() {
	if (this->mesh != nullptr) {
		delete this->mesh;
	}
	if (this->rigidAnimationDatas != nullptr) {
		for (auto rigidAnimationData_it = this->rigidAnimationDatas->begin(); rigidAnimationData_it != this->rigidAnimationDatas->end(); ++rigidAnimationData_it) {
			RigidAnimationData* rigidAnimationData = *rigidAnimationData_it;
			delete rigidAnimationData;
		}
		this->rigidAnimationDatas->clear();
	}
}

void Model::SetName(std::string name) {
	this->name = name;

	// Clean it up a little:
	size_t colonPos = this->name.find(":");
	while (colonPos != std::string::npos) {
		this->name.replace(colonPos, 1, "_");
		colonPos = this->name.find(":");
	}
}
