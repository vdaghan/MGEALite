// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Wavelet/HaarWavelet.h"

#include <array>
#include <cmath>

//std::vector<double> haarWaveletEncode(std::vector<double> const & input) {
//	std::array<double, 2> const c = { 7.071067811865475E-01, 7.071067811865475E-01 };
//
//	size_t const numGenes(input.size());
//
//	size_t m(numGenes);
//	std::vector<double> y(input);
//	std::vector<double> z(numGenes, 0.0);
//
//	while (2 <= m) {
//		m = m / 2;
//
//		for (int i = 0; i < m; i++) {
//			z[i] = c[0] * (y[2 * i] + y[2 * i + 1]);
//			z[i + m] = c[1] * (y[2 * i] - y[2 * i + 1]);
//		}
//
//		for (int i = 0; i < 2 * m; i++) {
//			y[i] = z[i];
//		}
//	}
//
//	return y;
//}

//std::vector<double> haarWaveletDecode(std::vector<double> const & input) {
//	std::array<double, 2> const c = { 7.071067811865475E-01, 7.071067811865475E-01 };
//
//	size_t const numGenes(input.size());
//
//	std::vector<double> x(input);
//	std::vector<double> z(numGenes, 0.0);
//
//	for (int m = 1; m * 2 <= numGenes; m *= 2) {
//		for (int i = 0; i < m; ++i) {
//			z[2 * i] = c[0] * (x[i] + x[i + m]);
//			z[2 * i + 1] = c[1] * (x[i] - x[i + m]);
//		}
//
//		for (int i = 0; i < 2 * m; ++i) {
//			x[i] = z[i];
//		}
//	}
//
//	return x;
//}

std::vector<double> haarWaveletEncode(std::vector<double> const & input) {
	std::size_t const size(input.size());
	std::vector<double> x(input);
	
	double sqrt2 = std::sqrt(2.0);

	std::vector<double> y(size, 0.0);

	// Determine K, the largest power of 2 such that K <= N.
	std::size_t k(1);
	while (k * 2 <= size) {
		k = k * 2;
	}

	while (1 < k) {
		k = k / 2;
		for (std::size_t i(0); i < k; ++i) {
			y[i] = (x[2 * i] + x[2 * i + 1]) / sqrt2;
			y[i + k] = (x[2 * i] - x[2 * i + 1]) / sqrt2;
		}
		for (std::size_t i(0); i < k * 2; ++i) {
			x[i] = y[i];
		}
	}

	return x;
}

std::vector<double> haarWaveletDecode(std::vector<double> const & input) {
	std::size_t const size(input.size());
	std::vector<double> x(input);

	double sqrt2 = std::sqrt(2.0);

	std::vector<double> y(size, 0.0);

	std::size_t k(1);
	while (k * 2 <= size) {
		for (std::size_t i(0); i < k; ++i) {
			y[2 * i] = (x[i] + x[i + k]) / sqrt2;
			y[2 * i + 1] = (x[i] - x[i + k]) / sqrt2;
		}
		for (std::size_t i(0); i < k * 2; ++i) {
			x[i] = y[i];
		}
		k *= 2;
	}

	return x;
}

std::vector<std::vector<double>> haarWaveletEncode(std::vector<std::vector<double>> const & input) {
	std::vector<std::vector<double>> retVal;
	size_t const numChromosomes(input.size());

	for (size_t chromosome(0); chromosome < numChromosomes; ++chromosome) {
		retVal.emplace_back(haarWaveletEncode(input[chromosome]));
	}

	return retVal;
}

std::vector<std::vector<double>> haarWaveletDecode(std::vector<std::vector<double>> const & input) {
	std::vector<std::vector<double>> retVal;
	size_t const numChromosomes(input.size());

	for (size_t i(0); i < numChromosomes; ++i) {
		retVal.emplace_back(haarWaveletDecode(input[i]));
	}

	return retVal;
}
