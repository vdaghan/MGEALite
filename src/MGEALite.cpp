// TODO: Read mapping between json tags and vector indices from a json file
// TODO: Read json input files

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "mgealite_version.h"
#include "Database.h"
#include "MotionGeneration/MotionGenerator.h"
	
#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

int main() {
	std::cout << "MGEALite version: " << getMGEALiteVersion() << std::endl;
	Database db("./data");

	MotionGenerator motionGenerator;

	return 0;
}
