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
