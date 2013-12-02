#include "Exporter/Definitions/Vertex.h"

Vertex::Vertex() {
	this->inUse = false;
	this->dup_vertex_index = -1;
	//
	// Create max empty influences:
	for (int i = 0; i < MAX_BONE_INFLUENCES; ++i) {
		BoneWeightInfluence* emptyInfluence = new BoneWeightInfluence();
		this->influences.push_back(emptyInfluence);
	}
}

void Vertex::AddBoneWeightInfluence(int boneIdx, double weight) {
	BoneWeightInfluence* influence = new BoneWeightInfluence();
	influence->boneIdx = boneIdx;
	influence->weight  = weight;

	if (this->influences.size() < MAX_BONE_INFLUENCES) {
		this->influences.push_back(influence);
	} else {
		// Find the current least influence:
		unsigned int leastInfluenceIdx = 0;
		for (unsigned int i = 0; i < this->influences.size(); ++i) {
			if (this->influences[i]->weight < this->influences[leastInfluenceIdx]->weight) {
				leastInfluenceIdx = i;
			}
		}

		// If the current least is lesser than the new influence, replace it with the new influence:
		if (this->influences[leastInfluenceIdx]->weight < influence->weight) {
			this->influences[leastInfluenceIdx]->boneIdx = influence->boneIdx;
			this->influences[leastInfluenceIdx]->weight  = influence->weight;
		}
		delete influence;
	}
}
