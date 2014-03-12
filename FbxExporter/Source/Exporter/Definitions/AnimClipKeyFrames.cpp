#include "Exporter/Definitions/AnimClipKeyframes.h"
#include "Exporter/Utilities/Utilities.h"

AnimClipKeyframes::AnimClipKeyframes() {
	this->startKeyframeNumber = 0;
	this->endKeyframeNumber   = 0;
}

std::vector<AnimClipKeyframes*> extractAnimclipKeyframeNumbersFromFile(std::string filePath) {
	std::vector<AnimClipKeyframes*> vectorOfAnimclipKeyframes;

	std::ifstream file(filePath.c_str());
	ASSERT(file.good(), "Able to open file %s, for reading", filePath.c_str());

	int numAnimclips;
	file >> numAnimclips;
	printf("\nAnimclipKeyframes file %s, has %d anim clips", filePath.c_str(), numAnimclips);

	for (unsigned int i = 0; i < numAnimclips; ++i) {
		AnimClipKeyframes* animclipKeyframes = new AnimClipKeyframes();
		//
		file >> animclipKeyframes->clipName;
		file >> animclipKeyframes->startKeyframeNumber >> animclipKeyframes->endKeyframeNumber;
		//
		vectorOfAnimclipKeyframes.push_back(animclipKeyframes);
	}

	return vectorOfAnimclipKeyframes;
}


