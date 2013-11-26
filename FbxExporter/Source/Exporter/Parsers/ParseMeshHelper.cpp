#include "Exporter/Parsers/ParseMeshHelper.h"
#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"
#include "Exporter/Definitions/Mesh.h"
#include "Exporter/Definitions/Polylist.h"
#include "Exporter/Utilities/Utilities.h"

bool hasUvTextures(FbxMesh* fbxMesh) {
	FbxStringList uvSetNames;
	fbxMesh->GetUVSetNames(uvSetNames);


	return (uvSetNames.GetCount() > 0);
}

std::string getUvSetName(FbxMesh* fbxMesh) {
	std::string uvSetName = "";
	if (hasUvTextures(fbxMesh)) {
		FbxStringList uvSetNames;
		fbxMesh->GetUVSetNames(uvSetNames);

		// TODO [Implement] Support more than 1 uv set
		const char* tempUvSetName = uvSetNames[0];
		uvSetName = tempUvSetName;
	}

	return uvSetName;
}

std::string getTextureFileName(FbxTexture* fbxTexture) {
	std::string returnString = "";

	FbxFileTexture *fbxFileTexture = FbxCast<FbxFileTexture>(fbxTexture);
	FbxProceduralTexture *fbxProceduralTexture = FbxCast<FbxProceduralTexture>(fbxTexture);

	if (fbxFileTexture != nullptr) {
		const char* textureFileName = fbxFileTexture->GetFileName();
		returnString = textureFileName;
		printf("\nFound texture filename: %s", returnString.c_str());
	}

	if (fbxProceduralTexture != nullptr) {
		printf("\nWARNING : Procedural textures found");
	}

	return returnString;
}

std::vector<std::string>* findAndGetTextureInfoByProperty(FbxProperty fbxTextureProperty, int midx) {
	std::vector<std::string>* textureFileNames = new std::vector<std::string>();

	if( fbxTextureProperty.IsValid() )
	{
		int numTextures = fbxTextureProperty.GetSrcObjectCount<FbxTexture>();

		for (int j = 0; j < numTextures; ++j) {

			// Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *fbxLayeredTexture = fbxTextureProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (fbxLayeredTexture) {

				int numLayeredTextures = fbxLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < numLayeredTextures; ++k) {

					FbxTexture* fbxTexture = fbxLayeredTexture->GetSrcObject<FbxTexture>(k);
					if (fbxTexture) {

						// NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
						// Why is that?  because one texture can be shared on different layered textures and might
						// have different blend modes.

						FbxLayeredTexture::EBlendMode lBlendMode;
						fbxLayeredTexture->GetTextureBlendMode(k, lBlendMode);
						std::string textureFileName = getTextureFileName(fbxTexture);
						textureFileNames->push_back(textureFileName);
					}
				}
			}
			else {
				// No layered texture simply get on the property
				FbxTexture* fbxTexture = fbxTextureProperty.GetSrcObject<FbxTexture>(j);
				if (fbxTexture) {
					// Display connected Material header only at the first time

					std::string textureFileName = getTextureFileName(fbxTexture);
					textureFileNames->push_back(textureFileName);
				}
			}
		}
	}

	return textureFileNames;
}

void getMaterialData(FbxSurfaceMaterial* fbxMaterial, Material* out_material) {
	FbxPropertyT<FbxDouble3> fbxDouble3;
	if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
		fbxDouble3 = ((FbxSurfacePhong *) fbxMaterial)->Ambient;
		out_material->ambientColor = glm::vec3(fbxDouble3.Get()[0], fbxDouble3.Get()[1], fbxDouble3.Get()[2]);
		fbxDouble3 = ((FbxSurfacePhong *) fbxMaterial)->Diffuse;
		out_material->diffuseColor = glm::vec3(fbxDouble3.Get()[0], fbxDouble3.Get()[1], fbxDouble3.Get()[2]);
		fbxDouble3 = ((FbxSurfacePhong *) fbxMaterial)->Specular;
		out_material->specularColor = glm::vec3(fbxDouble3.Get()[0], fbxDouble3.Get()[1], fbxDouble3.Get()[2]);
	} else
	if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
		fbxDouble3 = ((FbxSurfaceLambert *) fbxMaterial)->Ambient;
		out_material->ambientColor = glm::vec3(fbxDouble3.Get()[0], fbxDouble3.Get()[1], fbxDouble3.Get()[2]);
		fbxDouble3 = ((FbxSurfaceLambert *) fbxMaterial)->Diffuse;
		out_material->diffuseColor = glm::vec3(fbxDouble3.Get()[0], fbxDouble3.Get()[1], fbxDouble3.Get()[2]);
		out_material->specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	} else {
		// Unknown material type:
		printf("\nWARNING: Unknown material type");
		out_material->ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);
		out_material->diffuseColor = glm::vec3(0.7f, 0.7f, 0.7f);
		out_material->specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	}
}

void getMaterialAndTextureFileNames(FbxGeometry* fbxGeometry, std::vector<std::string>* out_textureFileNames,
																		Material* out_material) {

	if (fbxGeometry->GetNode() == nullptr) {
		return;
	}

	int numMaterials = fbxGeometry->GetNode()->GetSrcObjectCount(FbxSurfaceMaterial::ClassId);
	for (int midx = 0; midx < numMaterials; ++midx){

		FbxSurfaceMaterial *fbxMaterial = (FbxSurfaceMaterial *)fbxGeometry->GetNode()->GetSrcObject(FbxSurfaceMaterial::ClassId, midx);

		if (fbxMaterial) {

			getMaterialData(fbxMaterial, out_material);

			// Go through all the possible textures
			int tidx;
			FBXSDK_FOR_EACH_TEXTURE(tidx) {
				FbxProperty fbxTextureProperty = fbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[tidx]);
				std::vector<std::string>* tempTextureFileNames = findAndGetTextureInfoByProperty(fbxTextureProperty, midx);
				for (int t = 0; t < tempTextureFileNames->size(); ++t) {
					out_textureFileNames->push_back(tempTextureFileNames->at(t));
				}
			}

		}//end if(fbxMaterial)

	}// end for materialIndex

	return;
}

int InsertVertexFromPolygonIntoMeshAndReturnIndex(FbxMesh* fbxMesh, int pidx, int vertexIndexInPolygon, bool hasTextures, const char* uvSetName, Mesh* mesh) {
	int controlPointIndex = fbxMesh->GetPolygonVertex(pidx, vertexIndexInPolygon);
	FbxVector4 controlPointFbxVec = fbxMesh->GetControlPointAt(controlPointIndex);
	FbxVector4 normalFbxVec;
	fbxMesh->GetPolygonVertexNormal(pidx, vertexIndexInPolygon, normalFbxVec);
	FbxVector2 uvFbxVec(0.0, 0.0);
	if (hasTextures) {
		bool unMapped;
		// TODO [Implement] Support more than 1 uv set
		fbxMesh->GetPolygonVertexUV(pidx, vertexIndexInPolygon, uvSetName, uvFbxVec, unMapped);
	}

	// Make a new vertex with all this data:
	Vertex* vertex = new Vertex();
	vertex->position = ConvertFbxVector4ToGlmVec3(controlPointFbxVec);
	vertex->normal = ConvertFbxVector4ToGlmVec3(normalFbxVec);
	vertex->texCoords = ConvertFbxVector2ToGlmVec2(uvFbxVec);

	// Insert it into the mesh:
	if (controlPointIndex == 7612) {
		static int qwe = 0;
		qwe++;
	}
	int newControlPointIndex = mesh->AddNewVertexAtIndex(vertex, controlPointIndex);
	return newControlPointIndex;
}

Mesh* ParseFbxMesh(FbxMesh* fbxMesh) {
	std::string meshUvSetName = "";
	bool hasTextures = hasUvTextures(fbxMesh);
	if (hasTextures) {
		meshUvSetName = getUvSetName(fbxMesh);
	}

    // Create a new Mesh:
	Mesh* mesh = new Mesh();
	// Create the minimum number of empty vertices needed for this mesh:
	{
		mesh->vertices->clear();
		int numControlPoints = fbxMesh->GetControlPointsCount();
		for (int i = 0; i < numControlPoints; ++i) {
			Vertex* emptyVertex = new Vertex();
			(*(mesh->vertices))[i] = emptyVertex;
		}
	}


	const char* meshName = fbxMesh->GetName();

	// Create a set of faces to hold the vertices and faces that we'll discover in the fbxMesh
	std::vector<Polylist*>* polylists = new std::vector<Polylist*>();
	Polylist* polylist = new Polylist();
	polylist->faces = new std::vector<Face*>();

    int numPolygons = fbxMesh->GetPolygonCount();
	// Run through all the polygons in the mesh:
    for (int pidx = 0; pidx < numPolygons; ++pidx) {

    	int numVertsInPolygon = fbxMesh->GetPolygonSize(pidx);
    	// Break polygon into NUM_VERTICES_PER_FACE(triangular) faces:
    	for (int i = 0 + 1; i < (numVertsInPolygon - (NUM_VERTICES_PER_FACE - 2)); i += (NUM_VERTICES_PER_FACE - 2)) {

    		// Make a new face with NUM_VERTICES_PER_FACE(3) control point indexes:
    		Face* face = new Face();
    		face->indices[0] = InsertVertexFromPolygonIntoMeshAndReturnIndex(fbxMesh, pidx, 0, hasTextures, meshUvSetName.c_str(), mesh);

    		for (int j = 1; j < NUM_VERTICES_PER_FACE; ++j) {
    			int vertexIndexInPolygon = i + j - 1;
    			face->indices[j] = InsertVertexFromPolygonIntoMeshAndReturnIndex(fbxMesh, pidx, vertexIndexInPolygon, hasTextures, meshUvSetName.c_str(), mesh);
    		}

    		polylist->faces->push_back(face);
    	}
    }

	std::vector<std::string>* textureFileNames = new std::vector<std::string>();
    getMaterialAndTextureFileNames(fbxMesh, textureFileNames, mesh->material);
    if (textureFileNames->size() > 0) {
    	printf("\nNumber of texture file names: %lu", textureFileNames->size());
    	// TODO [Implement] Support multiple texture files:
    	mesh->textureFileName = textureFileNames->at(0);
    }

    polylists->push_back(polylist);

    // Finally, put things into the mesh:
	mesh->name = meshName;
	mesh->polylists = polylists;

	return mesh;
}
