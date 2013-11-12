#ifndef SCENE_H
#define SCENE_H

#include "Exporter/Definitions/Model.h"

#include <string>
#include <vector>

class Scene {
public:
	Scene();
	~Scene();

	std::string name;
	std::vector<Model*>* models;

private:
};

#endif // SCENE_H
