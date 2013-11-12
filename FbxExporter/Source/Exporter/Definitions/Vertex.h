#ifndef VERTEX_H
#define VERTEX_H

#include "Libraries/glm/glm.hpp"

class Vertex {
public:
	Vertex();

	bool inUse;

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	// Needed to be able chain together "duplicate" vertices (typically at texture seams)
	// which share position [and normal] but differ in texCoords
	// This chaining of dups makes it easy to search if a duplicate already exists for a given vertex_index
	int dup_vertex_index;

private:
};

#endif // VERTEX_H
