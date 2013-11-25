#include "Exporter/Parsers/ParseAnimations/RigidAnimationData.h"
#include "Exporter/Utilities/Utilities.h"

#include <algorithm>

void setFieldInPropertyVector(RigidAnimationPropertyField_t field, glm::vec3& propertyVector, float fieldValue) {
	switch (field) {
	case PROPERTY_FIELD_X:
		propertyVector.x = fieldValue;
		break;
	case PROPERTY_FIELD_Y:
		propertyVector.y = fieldValue;
		break;
	case PROPERTY_FIELD_Z:
		propertyVector.z = fieldValue;
		break;
	}
}

void RigidAnimationData::AddKeyframeForPropertyField(RigidAnimationProperty_t animation_property, RigidAnimationPropertyField_t field,
													 float value, float time) {

	RigidAnimationKeyframe* keyframe = this->getKeyframeAtTime(time);
	VERIFY(keyframe != nullptr, "Got a valid keyframe");

	switch (animation_property) {
	case PROPERTY_TRANSLATION:
		setFieldInPropertyVector(field, keyframe->translation, value);
		break;
	case PROPERTY_ROTATION:
		setFieldInPropertyVector(field, keyframe->rotation, value);
		break;
	case PROPERTY_SCALING:
		setFieldInPropertyVector(field, keyframe->scaling, value);
		break;
	}

}

void RigidAnimationData::SetName(std::string newName) {
	size_t whitespacePos = std::string::npos;
	while ((whitespacePos = newName.find(' ')) != std::string::npos) {
		newName.replace(whitespacePos, 1, "_");
	}
	this->name = newName;
}

RigidAnimationKeyframe* RigidAnimationData::GetKeyframeAtIndex(unsigned int index) {
	if (index < this->keyframes.size()) {
		return this->keyframes[index];
	}
	return nullptr;
}

#define DEFAULT_TOLERANCE_PERCENTAGE 0.2f // 0.2%
bool areFloatsApproximatelyEqual(float a, float b, float tolerancePercentage = DEFAULT_TOLERANCE_PERCENTAGE) {
	float tolerance = abs(std::max(std::max(a, b), 0.1f) * (tolerancePercentage / 100.0f));

	if (a >= (b - tolerance)  &&  a <= (b + tolerance)) {
		return true;
	}
	return false;
}

RigidAnimationKeyframe* RigidAnimationData::getKeyframeAtTime(float time) {
	// Look for existing keyframes at that time:
	for (int i = 0; i < this->keyframes.size(); ++i) {
		RigidAnimationKeyframe* keyframe = this->keyframes[i];
		if (areFloatsApproximatelyEqual(time, keyframe->time)) {
			return keyframe;
		}
	}

	// No existing keyframe found. Must add a new one:
	RigidAnimationKeyframe* newKeyframe = new RigidAnimationKeyframe();
	newKeyframe->time = time;
	// Insert it at the appropriate place:
	auto keyframe_it = this->keyframes.begin();
	for (; keyframe_it != this->keyframes.end(); ++keyframe_it) {
		RigidAnimationKeyframe* keyframe = *keyframe_it;
		if (keyframe->time > newKeyframe->time) {
			break;
		}
	}
	this->keyframes.insert(keyframe_it, newKeyframe);

	return newKeyframe;
}
