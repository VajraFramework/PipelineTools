#ifndef WriteArmature_h__
#define WriteArmature_h__

#include "Exporter/Writers/WriteScene.h"

#include <fstream>

// Forward Declarations:
class Armature;

void exportArmature(Armature* armature, std::ofstream& file);

#endif // WriteArmature_h__
