#include "Exporter/Parsers/ParseAnimationHelper.h"

void ParseSceneForAnimations(FbxScene* fbxScene) {
	printf("\nParsing scene for animations");
	int numStacks = fbxScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack));

	printf("\nNumber of animation stacks found: %d", numStacks);
}
