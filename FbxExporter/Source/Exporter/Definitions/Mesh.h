#ifndef MESH_H
#define MESH_H

#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Definitions/Vertex.h"

#include "Libraries/glm/glm.hpp"

#include <map>
#include <string>
#include <vector>

// Forward Declarations:
class Armature;
class Material;

class Mesh {
public:
	Mesh();
	~Mesh();

	std::string name;

	// Returns new vertex index which may or may not be the same as the one provided
	int AddNewVertexAtIndex(Vertex* newVertex, int suggestedVertexIndex);
	std::map<int /* vertex index */, Vertex*>* vertices;
	void AddBoneWeightInfluenceToVertexAtIndex(int vertexIndex, int boneIndex, double weight);

	std::vector<Polylist*>* polylists;

	// TODO [Implement] Move this inside the material inside each polylist
	std::string textureFileName;
	Material* material;

	Armature* armature;

private:
};

#endif // MESH_H
