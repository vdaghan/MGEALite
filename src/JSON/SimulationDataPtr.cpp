// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "JSON/SimulationDataPtr.h"

#include "DEvA/JSON/Common.h"

#include <filesystem>

namespace MGEA {
	[[nodiscard]] DEvA::Maybe<SimulationDataPtr> importSimulationDataPtrFromFile(std::filesystem::path const & filename) {
		if (not std::filesystem::exists(filename)) {
			return std::unexpected(DEvA::ErrorCode::FileNotFound);
		}
		std::ifstream f;
		f.open(filename, std::ios_base::in);
		if (not f.is_open()) {
			return std::unexpected(DEvA::ErrorCode::FileNotOpen);
		}
		try {
			JSON const parseResult = JSON::parse(f);
			f.close();
			SimulationDataPtr retVal = parseResult;
			if (retVal->error) {
				return std::unexpected(DEvA::ErrorCode::InvalidTransform);
			}
			return retVal;
		} catch (const std::exception &) {
			f.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return std::unexpected(DEvA::ErrorCode::ParseError);
		}
    }
    
	DEvA::ErrorCode exportSimulationDataPtrToFile(SimulationDataPtr const & t, std::filesystem::path const & filename) {
 		JSON const j = t;
		std::ofstream f(filename);
		if (!f.is_open()) {
			return DEvA::ErrorCode::FileNotOpen;
		}
		f << std::setw(4) << j << std::endl;
		return DEvA::ErrorCode::OK;
    }
}