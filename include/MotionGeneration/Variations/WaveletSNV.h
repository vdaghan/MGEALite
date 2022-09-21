#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "SimulationData.h"

std::array<size_t, 2> stageToIndices(size_t numJoints, size_t stage);
std::vector<double> haarWaveletEncodeSingle(std::vector<double> const & input);
std::vector<double> haarWaveletDecodeSingle(std::vector<double> const & input);
std::vector<std::vector<double>> haarWaveletEncode(const std::vector<std::vector<double>> & input);
std::vector<std::vector<double>> haarWaveletDecode(const std::vector<std::vector<double>> & input);
SimulationDataPtrs waveletSNV(MotionParameters const &, SimulationDataPtrs);
