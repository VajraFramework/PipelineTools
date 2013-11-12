#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include "Libraries/glm/glm.hpp"

// Forward Declarations:
class Mesh;

class Model {
public:
	Model();
	~Model();

	std::string name;

	Mesh* mesh;

	glm::vec3 initialPosition;
	glm::vec3 initialRotation;
	glm::vec3 initialScale;

private:
};

#endif // MODEL_H
