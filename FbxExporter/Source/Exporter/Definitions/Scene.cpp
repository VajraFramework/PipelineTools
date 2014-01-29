#include "Exporter/Definitions/Scene.h"

#include <fstream>

Scene::Scene() {
	this->models = new std::vector<Model*>();
}

Scene::~Scene() {
	if (this->models != nullptr) {
		this->models->clear();
		delete this->models;
	}
}

Model* Scene::GetModelByModelName(std::string name) {
	for (auto it = this->models->begin(); it != this->models->end(); ++it) {
		Model* model = *it;
		if (model->name == name) {
			return model;
		}
	}
	return nullptr;
}

void Scene::DEBUG_ExportNamesOfAllObjectsInScene() {
	std::ofstream file("./modelnamesinscene");
	for (auto it = this->models->begin(); it != this->models->end(); ++it) {
		Model* model = *it;
		file << model->name.c_str();
		file << std::endl;
	}
}
