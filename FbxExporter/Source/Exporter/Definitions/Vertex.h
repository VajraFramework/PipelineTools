#ifndef VERTEX_H
#define VERTEX_H

#include "Libraries/glm/glm.hpp"

#include <vector>

#define MAX_BONE_INFLUENCES 4

struct BoneWeightInfluence {
	BoneWeightInfluence() {
		this->boneIdx = 0;
		this->weight = 0.0;
	}

	int boneIdx;
	double weight;
};

class Vertex {
public:
	Vertex();

	bool inUse;

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	std::vector<BoneWeightInfluence*> influences;

	// Needed to be able chain together "duplicate" vertices (typically at texture seams)
	// which share position [and normal] but differ in texCoords
	// This chaining of dups makes it easy to search if a duplicate already exists for a given vertex_index
	int dup_vertex_index;

	void AddBoneWeightInfluence(int boneIdx, double weight);

private:
};

#endif // VERTEX_H
