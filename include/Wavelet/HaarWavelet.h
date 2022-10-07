#pragma once

#include <vector>

// https://people.math.sc.edu/Burkardt/cpp_src/haar/haar.html
std::vector<double> haarWaveletEncode(std::vector<double> const & input);
std::vector<double> haarWaveletDecode(std::vector<double> const & input);
std::vector<std::vector<double>> haarWaveletEncode(std::vector<std::vector<double>> const & input);
std::vector<std::vector<double>> haarWaveletDecode(std::vector<std::vector<double>> const & input);
