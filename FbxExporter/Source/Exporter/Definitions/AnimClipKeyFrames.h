#ifndef ANIMCLIP_KEYFRAMES_H
#define ANIMCLIP_KEYFRAMES_H

#include "Exporter/Definitions/Declarations.h"

#include <fstream>
#include <string>
#include <vector>

class AnimClipKeyframes {
public:
	AnimClipKeyframes();

	std::string clipName;
	unsigned int startKeyframeNumber;
	unsigned int endKeyframeNumber;

private:
};

std::vector<AnimClipKeyframes*> extractAnimclipKeyframeNumbersFromFile(std::string filePath);

#endif // ANIMCLIP_KEYFRAMES_H
