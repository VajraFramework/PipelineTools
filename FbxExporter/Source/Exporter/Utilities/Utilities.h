#ifndef UTILITIES_H
#define UTILITIES_H

#include "FbxSdk/include/fbxsdk.h"
#include "Libraries/glm/glm.hpp"

#include <string>
#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
#define ASSERT(x, fmt, ...) \
	ASSERT_internal(__FILE__, __LINE__, __FUNCTION__,	\
					x, fmt , ##__VA_ARGS__)

#define ASSERT_LOG(x, fmt, ...) \
	ASSERT_LOG_internal(__FILE__, __LINE__, __FUNCTION__,	\
					x, fmt , ##__VA_ARGS__)
#else
////////////////////////////////////////////////////////////////////////////////
// Asserts are compiled out in non-debug builds:
#define ASSERT(x, fmt, ...)
#define ASSERT_LOG(x, fmt, ...)
#endif // DEBUG
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#define VERIFY(x, fmt, ...) \
	ASSERT_internal(__FILE__, __LINE__, __FUNCTION__,	\
					x, fmt , ##__VA_ARGS__)

#define VERIFY_LOG(x, fmt, ...) \
	ASSERT_LOG_internal(__FILE__, __LINE__, __FUNCTION__,	\
					x, fmt , ##__VA_ARGS__)
////////////////////////////////////////////////////////////////////////////////
	

////////////////////////////////////////////////////////////////////////////////
void ASSERT_internal(const char* file, int lineNumber, const char* functionName, bool expression, const char* fmt, ...);
void ASSERT_LOG_internal(const char* file, int lineNumber, const char* functionName, bool expression, const char* fmt, ...);
////////////////////////////////////////////////////////////////////////////////

glm::vec2 ConvertFbxVector2ToGlmVec2(FbxVector2 fv);
glm::vec3 ConvertFbxVector4ToGlmVec3(FbxVector4 fv);
glm::vec4 ConvertFbxVector4ToGlmVec4(FbxVector4 fv);
//
glm::mat4x4 ConvertFbxMatrixToGlmMat4x4(FbxMatrix fm);

void printGlmVec2(glm::vec2 v);
void printGlmVec3(glm::vec3 v);
void printGlmVec4(glm::vec4 v);
//
void printGlmMat4x4(glm::mat4x4 m);

void PrintVectorOfVec3(std::vector<glm::vec3>* vectorOfVec3, std::string printAs);
void PrintVectorOfVec2(std::vector<glm::vec2>* vectorOfVec2, std::string printAs);

// Forward Declaration:
class Mesh;
void PrintMesh(Mesh* mesh);

void WriteGlmVec3ToFile(glm::vec3 v, std::ofstream& file);
void WriteGlmVec2ToFile(glm::vec2 v, std::ofstream& file);
void WriteVectorOfVec3ToFile(std::vector<glm::vec3>* vectorOfVec3, std::ofstream& file);
void WriteVectorOfVec2ToFile(std::vector<glm::vec2>* vectorOfVec2, std::ofstream& file);
//
void WriteGlmMat4x4ToFile(glm::mat4x4 m, std::ofstream& file);

#define DEFAULT_TOLERANCE_PERCENTAGE 0.2f // 0.2%
bool areFloatsApproximatelyEqual(float a, float b, float tolerancePercentage = DEFAULT_TOLERANCE_PERCENTAGE);

#endif // UTILITIES_H
