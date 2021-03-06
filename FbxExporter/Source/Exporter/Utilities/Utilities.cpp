#include "Exporter/Utilities/Utilities.h"
#include "Exporter/Definitions/Mesh.h"

#include <assert.h>
#include <cstdarg>
#include <cstdio>
#include <fstream>


#if defined(PLATFORM_DESKTOP) && defined(PLATFORM_LINUX)
#include <execinfo.h>
#include <unistd.h>
void printStackTrace() {
	void *array[10];
	size_t size;

	size = backtrace(array, 10);

	printf("\nStackTrace: \n");
	backtrace_symbols_fd(array, size, STDOUT_FILENO);
}
#else
void printStackTrace() {
	// TODO [Implement] printStackTrace() for non-linux platforms
}
#endif // PLATFORM_DESKTOP && PLATFORM_LINUX

void ASSERT_internal(const char* file, int lineNumber, const char* functionName, bool expression, const char* fmt, ...) {
	if (!expression) {
		va_list args;
		va_start(args, fmt);

		printf("\n! FAILED ASSERT: ");
		printf("\nIn file %s:%d: In function \'%s()\'\n    Failed while asserting that >> \"", file, lineNumber, functionName);
		vprintf(fmt, args);
		printf("\"\n");

		printStackTrace();
		va_end(args);

		assert(0);
	}
}

void ASSERT_LOG_internal(const char* file, int lineNumber, const char* functionName, bool expression, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	if (!expression) {
		printf("\n! FAILED ASSERT: ");
		printf("\nIn file %s:%d: In function \'%s()\'\n    Failed while asserting that >> \"", file, lineNumber, functionName);
		vprintf(fmt, args);
		printf("\"\n");

		printStackTrace();
		assert(0);

	} else {
		vprintf(fmt, args);
	}

	va_end(args);
}

glm::vec2 ConvertFbxVector2ToGlmVec2(FbxVector2 fv) {
	glm::vec2 v;
	v.x = fv.mData[0];
	v.y = fv.mData[1];

	return v;
}

glm::vec4 ConvertFbxVector4ToGlmVec4(FbxVector4 fv) {
	glm::vec4 v;
	v.x = fv.mData[0];
	v.y = fv.mData[1];
	v.z = fv.mData[2];
	v.w = fv.mData[3];

	return v;
}

glm::vec3 ConvertFbxVector4ToGlmVec3(FbxVector4 fv) {
	glm::vec3 v;
	v.x = fv.mData[0];
	v.y = fv.mData[1];
	v.z = fv.mData[2];

	return v;
}

glm::mat4x4 ConvertFbxMatrixToGlmMat4x4(FbxMatrix fm) {
	glm::mat4x4 m;
	glm::vec4 rows[4];
	for (int r = 0; r < 4; ++r) {
		FbxVector4 frow = fm.GetRow(r);
		rows[r] = ConvertFbxVector4ToGlmVec4(frow);
	}

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			m[r][c] = rows[r][c];
		}
	}

	return m;
}

void printGlmVec2(glm::vec2 v) {
	printf("%f %f", v.x, v.y);
}

void printGlmVec3(glm::vec3 v) {
	printf("%f %f %f", v.x, v.y, v.z);
}

void printGlmVec4(glm::vec4 v) {
	printf("%f %f %f %f", v.x, v.y, v.z, v.w);
}

void printGlmMat4x4(glm::mat4x4 m) {
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			printf("%f ", m[r][c]);
		}
		printf("\n");
	}
}

void PrintVectorOfVec3(std::vector<glm::vec3>* vectorOfVec3, std::string printAs) {
	printf("\n%s:", printAs.c_str());
	for (int i = 0; i < vectorOfVec3->size(); ++i) {
		printf("\n%d: ", i);
		printGlmVec3(vectorOfVec3->at(i));
	}
	printf("\n");
}

void PrintVectorOfVec2(std::vector<glm::vec2>* vectorOfVec2, std::string printAs) {
	printf("\n%s:", printAs.c_str());
	for (int i = 0; i < vectorOfVec2->size(); ++i) {
		printf("\n%d: ", i);
		printGlmVec2(vectorOfVec2->at(i));
	}
	printf("\n");
}

#define DEBUG_PRINT_VERTEXES_TO_CONSOLE 0

void PrintMesh(Mesh* mesh) {
	printf("\nMesh: %s", mesh->name.c_str());

	printf("\nNumber of vertices: %lu", mesh->vertices->size());

	std::vector<glm::vec3>* positions = new std::vector<glm::vec3>();
	std::vector<glm::vec3>* normals = new std::vector<glm::vec3>();
	std::vector<glm::vec2>* texCoords = new std::vector<glm::vec2>();

	int numVertices = mesh->vertices->size();

	for (int vidx = 0; vidx < numVertices; ++vidx) {
		Vertex* vertex = mesh->vertices->at(vidx);

		positions->push_back(vertex->position);
		normals->push_back(vertex->normal);
		texCoords->push_back(vertex->texCoords);
	}

#if DEBUG_PRINT_VERTEXES_TO_CONSOLE
	PrintVectorOfVec3(positions, "Positions");
	PrintVectorOfVec3(normals, "Normals");
	PrintVectorOfVec2(texCoords, "TexCoords");
#endif

}

void WriteGlmVec3ToFile(glm::vec3 v, std::ofstream& file) {
	file << v.x << " " << v.y << " " << v.z;
}

void WriteGlmVec2ToFile(glm::vec2 v, std::ofstream& file) {
	file << v.x << " " << v.y;
}

void WriteVectorOfVec3ToFile(std::vector<glm::vec3>* vectorOfVec3, std::ofstream& file) {
	file << vectorOfVec3->size() << "\n";
	for (int i = 0; i < vectorOfVec3->size(); ++i) {
		WriteGlmVec3ToFile(vectorOfVec3->at(i), file);
		file << "\n";
	}
	file << "\n";
}

void WriteVectorOfVec2ToFile(std::vector<glm::vec2>* vectorOfVec2, std::ofstream& file) {
	file << vectorOfVec2->size() << "\n";
	for (int i = 0; i < vectorOfVec2->size(); ++i) {
		WriteGlmVec2ToFile(vectorOfVec2->at(i), file);
		file << "\n";
	}
	file << "\n";
}

void WriteGlmMat4x4ToFile(glm::mat4x4 m, std::ofstream& file) {
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			file << m[r][c] << " ";
		}
		file << "\n";
	}
	file << "\n";
}

bool areFloatsApproximatelyEqual(float a, float b, float tolerancePercentage) {
	float tolerance = abs(std::max(std::max(a, b), 0.1f) * (tolerancePercentage / 100.0f));

	if (a >= (b - tolerance)  &&  a <= (b + tolerance)) {
		return true;
	}
	return false;
}
