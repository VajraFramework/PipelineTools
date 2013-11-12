#include "Exporter/Definitions/Face.h"

Face::Face() {
	for (int i = 0; i < NUM_VERTICES_PER_FACE; ++i) {
		this->indices[i] = 0;
	}
}
