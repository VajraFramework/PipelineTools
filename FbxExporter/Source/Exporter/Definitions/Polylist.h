#ifndef POLYLIST_H
#define POLYLIST_H

#include "Exporter/Definitions/Face.h"
#include "Exporter/Definitions/Material.h"

#include <vector>

class Polylist {
public:
	Material material;
	std::vector<Face*>* faces;

private:
};

#endif // POLYLIST_H
