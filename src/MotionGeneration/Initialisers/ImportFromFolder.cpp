// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "JSON/SimulationDataPtr.h"
#include "MotionGeneration/Specification.h"
#include "DEvA/JSON/Individual.h"

#include <algorithm>
#include <filesystem>
#include <list>
#include <string>

namespace MGEA {
	SimulationDataPtrs genesisImportFromFolder(MotionParameters motionParameters, DEvA::ParameterMap parameters) {
		std::string folder("./");
		if (parameters.contains("folder")) {
			folder = parameters.at("folder").get<std::string>();
		}

		std::filesystem::path genesisPath(folder);
		std::list<std::filesystem::path> simulationDataPaths{};

		if (parameters.contains("files")) {
			std::vector<std::string> files = parameters.at("files").get<std::vector<std::string>>();
			for (auto const & file : files) {
				simulationDataPaths.push_back(genesisPath / file);
			}
		} else {
			for (auto & d : std::filesystem::directory_iterator{ genesisPath }) {
				if (d.path() == genesisPath) {
					continue;
				}
				std::string const fileNameStem = d.path().stem().string();
				std::string const fileNameExtension = d.path().extension().string();

				if (fileNameExtension.empty() or fileNameExtension.substr(1) != "deva") {
					continue;
				}

				simulationDataPaths.push_back(d.path());
			}
		}


		SimulationDataPtrs simDataPtrs{};
		for (auto const & simDataPath : simulationDataPaths) {
			auto maybeIndividual(DEvA::importFromFile<Spec>(simDataPath));
			if (not maybeIndividual) {
				continue;
			}
			simDataPtrs.push_back(maybeIndividual.value().genotype);
		}
		
		return simDataPtrs;
	}
}
