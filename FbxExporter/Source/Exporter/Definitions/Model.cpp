#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Model.h"

Model::Model() {
	this->mesh = nullptr;
}

Model::~Model() {
	if (this->mesh != nullptr) {
		delete this->mesh;
	}
}
