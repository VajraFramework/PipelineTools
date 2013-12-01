#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include "Libraries/glm/glm.hpp"

// Forward Declarations:
class Mesh;
class RigidAnimationData;

class Model {
public:
	Model();
	~Model();

	void SetName(std::string name);
	std::string name;

	Mesh* mesh;
	std::vector<RigidAnimationData*>* rigidAnimationDatas;

	glm::vec3 initialPosition;
	glm::vec3 initialRotation;
	glm::vec3 initialScale;

private:
};

#endif // MODEL_H
