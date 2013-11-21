#ifndef RIGID_ANIMATION_DATA_H
#define RIGID_ANIMATION_DATA_H

#include "Libraries/glm/glm.hpp"

#include <vector>

enum RigidAnimationProperty_t {
	PROPERTY_TRANSLATION,
	PROPERTY_ROTATION,
	PROPERTY_SCALING,
};

enum RigidAnimationPropertyField_t {
	PROPERTY_FIELD_X,
	PROPERTY_FIELD_Y,
	PROPERTY_FIELD_Z,
};

// Forward Declarations:
class RigidAnimationKeyframe;

class RigidAnimationData {
public:
	void AddKeyframeForPropertyField(RigidAnimationProperty_t property, RigidAnimationPropertyField_t field, float value, float time);

	int GetNumKeyframes() { return this->keyframes.size(); }
	RigidAnimationKeyframe* GetKeyframeAtIndex(unsigned int index);

private:

	// Utility Functions:
	RigidAnimationKeyframe* getKeyframeAtTime(float time);

	std::vector<RigidAnimationKeyframe*> keyframes;
};

#endif // RIGID_ANIMATION_DATA_H
