#include "Exporter/Definitions/Scene.h"

Scene::Scene() {
	this->models = new std::vector<Model*>();
}

Scene::~Scene() {
	if (this->models != nullptr) {
		this->models->clear();
		delete this->models;
	}
}
