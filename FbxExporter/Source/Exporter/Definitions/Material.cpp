#include "Exporter/Definitions/Material.h"

Material::Material() {
	this->init();
}

Material::~Material() {
	this->destroy();
}

void Material::init() {
	this->ambientColor  = glm::vec3(0.0f, 1.0f, 0.0f);
	this->diffuseColor  = glm::vec3(0.0f, 1.0f, 0.0f);
	this->specularColor = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Material::destroy() {
}
