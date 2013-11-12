#ifndef PARSE_MESH_HELPER_H
#define PARSE_MESH_HELPER_H

#include "FbxSdk/include/fbxsdk.h"
#include "Libraries/glm/glm.hpp"

#include <vector>

// Forward Declarations:
class Mesh;
class Polylist;

Mesh* ParseFbxMesh(FbxMesh* fbxMesh);

#endif // PARSE_MESH_HELPER_H
