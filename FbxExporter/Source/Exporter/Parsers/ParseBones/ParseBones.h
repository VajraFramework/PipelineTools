#ifndef PARSE_BONES_H
#define PARSE_BONES_H

#include "Exporter/Common/Common.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Vertex.h"
#include "Exporter/Parsers/ParseBones/Definitions.h"
#include "FbxSdk/include/fbxsdk.h"

#include "Libraries/glm/glm.hpp"

#include <string>
#include <vector>

// Armature* ParseBones(FbxScene* FbxScene);
Armature* ParseSkinClusters(FbxGeometry* fbxGeometry);
void AddBoneWeightsToMesh(Armature* armature, Mesh* mesh);

#endif // PARSE_BONES_H
