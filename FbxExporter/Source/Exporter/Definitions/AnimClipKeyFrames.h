#ifndef ANIMCLIP_KEYFRAMES_H
#define ANIMCLIP_KEYFRAMES_H

#include "Exporter/Definitions/Declarations.h"

#include <string>

class AnimClipKeyframes {
public:
	AnimClipKeyframes();

	std::string clipName;
	unsigned int startKeyframeNumber;
	unsigned int endKeyframeNumber;

private:
};

#endif // ANIMCLIP_KEYFRAMES_H
