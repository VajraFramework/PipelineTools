#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Utilities/Utilities.h"

// TODO [Cleanup] Move this somewhere else
const float EPSILON = 0.0001f;
bool floatsEqual(float f1, float f2) {
	return ( f1 <= (f2 + EPSILON)  &&  f1 >= (f2 - EPSILON) );
}
bool vec3sEqual(glm::vec3 v1, glm::vec3 v2) {
	return ( floatsEqual(v1.x, v2.x) &&
			 floatsEqual(v1.y, v2.y) &&
			 floatsEqual(v1.z, v2.z) );
}
bool vec2sEqual(glm::vec2 v1, glm::vec2 v2) {
	return ( floatsEqual(v1.x, v2.x) &&
			 floatsEqual(v1.y, v2.y) );
}

Mesh::Mesh() {
	this->name = "";
	this->vertices = new std::map<int, Vertex*>();
}

Mesh::~Mesh() {
	if (this->vertices != nullptr) {
		this->vertices->clear();
		delete this->vertices;
	}
}

int Mesh::AddNewVertexAtIndex(Vertex* newVertex, int suggestedVertexIndex) {
	newVertex->inUse = true;

	if (this->vertices->find(suggestedVertexIndex) == this->vertices->end() ||
		this->vertices->at(suggestedVertexIndex)->inUse == false) {
		// No such vertex already exists
		// Create as new vertex at suggested index:
		(*(this->vertices))[suggestedVertexIndex] = newVertex;
		return suggestedVertexIndex;

	} else {
		// Vertex at suggested index already exists, walk the chain of dup vertexes
		// until we reach the end of this chain, or until we find a perfect match:
		int sourceVertexIndex = suggestedVertexIndex;
		bool perfectMatchFound = false;    bool endOfDupChainReached = false;
		while (!perfectMatchFound && !endOfDupChainReached) {
			Vertex* sourceVertex = this->vertices->at(sourceVertexIndex);
			if (vec3sEqual(sourceVertex->position, newVertex->position) &&
				vec3sEqual(sourceVertex->normal, newVertex->normal)     &&
				vec2sEqual(sourceVertex->texCoords, newVertex->texCoords)) {
				// Perfect match found! Duplicate vertex
				perfectMatchFound = true;
				break;
			}
			endOfDupChainReached = (this->vertices->at(sourceVertexIndex)->dup_vertex_index == -1);
			if (!endOfDupChainReached) {
				sourceVertexIndex = this->vertices->at(sourceVertexIndex)->dup_vertex_index;
			}
		}

		// We should now be at the end of the chain, or a perfect match was found:
		if (perfectMatchFound) {
			// Nothing more to do here. Return the vertex index of the perfect match
			return sourceVertexIndex;

		} else {
			// No match found. Probably a genuine semi-dup vertex at a texture seam:
			// Add as a new vertex at the end of the map of vertices:
			int newIndexToInsertAt = this->vertices->size();
			(*(this->vertices))[newIndexToInsertAt] = newVertex;
			// Set the dup chain's end to include this newly added vertex now:
			this->vertices->at(sourceVertexIndex)->dup_vertex_index = newIndexToInsertAt;
			// Return the newly added vertex index:
			return newIndexToInsertAt;
		}
	}

	VERIFY(0, "\nUnreachable code");
	return -1;
}
