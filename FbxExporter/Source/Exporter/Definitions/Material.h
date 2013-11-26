#ifndef MATERIAL_H
#define MATERIAL_H

#include "Libraries/glm/glm.hpp"

#include <string>

class Material {
public:
	Material();
	~Material();

	std::string name;

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

private:
	void init();
	void destroy();
};

#endif // MATERIAL_H
