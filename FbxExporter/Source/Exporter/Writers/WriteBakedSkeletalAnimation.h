#ifndef WriteBakedSkeletalAnimation_h__
#define WriteBakedSkeletalAnimation_h__

#include "Exporter/Writers/WriteScene.h"

#include <fstream>

// Forward Declarations:
class Armature;

void exportBakedSkeletalAnimation(Armature* armature, std::ofstream& file);

#endif // WriteBakedSkeletalAnimation_h__