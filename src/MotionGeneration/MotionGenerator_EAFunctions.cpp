#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/ParentSelectors/ParentSelectors.h"
#include "MotionGeneration/Variations/Variations.h"
#include "Wavelet/HaarWavelet.h"

#include "Logging/SpdlogCommon.h"

#include <DTimer/DTimer.h>

#include <cmath>
#include <valarray>

void MotionGenerator::createVariationFunctors() {
	Spec::SVariationFunctor variationFunctorCrossoverAll;
	variationFunctorCrossoverAll.name = "CrossoverAll";
	variationFunctorCrossoverAll.numberOfParents = 2;
	variationFunctorCrossoverAll.parentSelectionFunction = std::bind_front(DEvA::StandardParentSelectors<Spec>::randomN<2>, "fitness");
	variationFunctorCrossoverAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariationWithGenotypeProxies, this, &MGEA::crossoverAll);
	variationFunctorCrossoverAll.probability = 1.0;
	variationFunctorCrossoverAll.removeParentsFromMatingPool = false;
	ea.registerVariationFunctor(variationFunctorCrossoverAll);
	
	//Spec::SVariationFunctor variationFunctorCrossoverSingle;
	//variationFunctorCrossoverSingle.name = "CrossoverSingle";
	//variationFunctorCrossoverSingle.numberOfParents = 2;
	//variationFunctorCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorCrossoverSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::crossoverSingle);
	//variationFunctorCrossoverSingle.probability = 1.0;
	//variationFunctorCrossoverSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorCrossoverSingle);
	
	//Spec::SVariationFunctor variationFunctorCutAndCrossfillAll;
	//variationFunctorCutAndCrossfillAll.name = "CutAndCrossfillAll";
	//variationFunctorCutAndCrossfillAll.numberOfParents = 2;
	//variationFunctorCutAndCrossfillAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorCutAndCrossfillAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::cutAndCrossfillAll);
	//variationFunctorCutAndCrossfillAll.probability = 1.0;
	//variationFunctorCutAndCrossfillAll.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorCutAndCrossfillAll);
	
	//Spec::SVariationFunctor variationFunctorCutAndCrossfillSingle;
	//variationFunctorCutAndCrossfillSingle.name = "CutAndCrossfillSingle";
	//variationFunctorCutAndCrossfillSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorCutAndCrossfillSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::cutAndCrossfillSingle);
	//variationFunctorCutAndCrossfillSingle.probability = 1.0;
	//variationFunctorCutAndCrossfillSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorCutAndCrossfillSingle);
	
	//Spec::SVariationFunctor variationFunctorDeletionAll;
	//variationFunctorDeletionAll.name = "DeletionAll";
	//variationFunctorDeletionAll.numberOfParents = 1;
	//variationFunctorDeletionAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::randomN<1>;
	//variationFunctorDeletionAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::deletionAll);
	//variationFunctorDeletionAll.probability = 1.0;
	//variationFunctorDeletionAll.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorDeletionAll);
	
	//Spec::SVariationFunctor variationFunctorDeletionSingle;
	//variationFunctorDeletionSingle.name = "DeletionSingle";
	//variationFunctorDeletionSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	//variationFunctorDeletionSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::deletionSingle);
	//variationFunctorDeletionSingle.probability = 0.2;
	//variationFunctorDeletionSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorDeletionSingle);
	
	//Spec::SVariationFunctor variationFunctorInsertionAll;
	//variationFunctorInsertionAll.name = "InsertionAll";
	//variationFunctorInsertionAll.numberOfParents = 1;
	//variationFunctorInsertionAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::randomN<1>;
	//variationFunctorInsertionAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::insertionAll);
	//variationFunctorInsertionAll.probability = 1.0;
	//variationFunctorInsertionAll.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorInsertionAll);
	
	//Spec::SVariationFunctor variationFunctorInsertionSingle;
	//variationFunctorInsertionSingle.name = "InsertionSingle";
	//variationFunctorInsertionSingle.numberOfParents = 1;
	//variationFunctorInsertionSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	//variationFunctorInsertionSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::insertionSingle);
	//variationFunctorInsertionSingle.probability = 1.0;
	//variationFunctorInsertionSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorInsertionSingle);
	
	//Spec::SVariationFunctor variationFunctorkPointCrossoverAll;
	//variationFunctorkPointCrossoverAll.name = "kPointCrossoverAll";
	//variationFunctorkPointCrossoverAll.numberOfParents = 2;
	//variationFunctorkPointCrossoverAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorkPointCrossoverAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::kPointCrossoverAll);
	//variationFunctorkPointCrossoverAll.probability = 1.0;
	//variationFunctorkPointCrossoverAll.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorkPointCrossoverAll);
	
	//Spec::SVariationFunctor variationFunctorkPointCrossoverSingle;
	//variationFunctorkPointCrossoverSingle.name = "kPointCrossoverSingle";
	//variationFunctorkPointCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorkPointCrossoverSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::kPointCrossoverSingle);
	//variationFunctorkPointCrossoverSingle.probability = 1.0;
	//variationFunctorkPointCrossoverSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorkPointCrossoverSingle);
	
	//Spec::SVariationFunctor variationFunctorSNV;
	//variationFunctorSNV.name = "SNV";
	//variationFunctorSNV.numberOfParents = 1;
	//variationFunctorSNV.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::randomN<1>;
	//variationFunctorSNV.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::snv);
	//variationFunctorSNV.probability = 1.0;
	//variationFunctorSNV.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorSNV);
	
	//Spec::SVariationFunctor variationFunctorUniformCrossoverAll;
	//variationFunctorUniformCrossoverAll.name = "UniformCrossoverAll";
	//variationFunctorUniformCrossoverAll.numberOfParents = 2;
	//variationFunctorUniformCrossoverAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorUniformCrossoverAll.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::uniformCrossoverAll);
	//variationFunctorUniformCrossoverAll.probability = 1.0;
	//variationFunctorUniformCrossoverAll.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorUniformCrossoverAll);
	
	//Spec::SVariationFunctor variationFunctorUniformCrossoverSingle;
	//variationFunctorUniformCrossoverSingle.name = "UniformCrossoverSingle";
	//variationFunctorUniformCrossoverSingle.numberOfParents = 2;
	//variationFunctorUniformCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorUniformCrossoverSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::uniformCrossoverSingle);
	//variationFunctorUniformCrossoverSingle.probability = 1.0;
	//variationFunctorUniformCrossoverSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorUniformCrossoverSingle);
	
	//Spec::SVariationFunctor variationFunctorWaveletSNV;
	//variationFunctorWaveletSNV.name = "WaveletSNV";
	//variationFunctorWaveletSNV.numberOfParents = 1;
	//variationFunctorWaveletSNV.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::randomN<1>;
	//variationFunctorWaveletSNV.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::waveletSNV);
	//variationFunctorWaveletSNV.probability = 1.0;
	//variationFunctorWaveletSNV.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorWaveletSNV);

	//Spec::SVariationFunctor variationFunctorHalfSineAsynchronous;
	//variationFunctorHalfSineAsynchronous.name = "HalfSineAsynchronous";
	//variationFunctorHalfSineAsynchronous.numberOfParents = 1;
	//variationFunctorHalfSineAsynchronous.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 10>;
	//variationFunctorHalfSineAsynchronous.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::halfSineAsynchronous);
	//variationFunctorHalfSineAsynchronous.probability = 1.0;
	//variationFunctorHalfSineAsynchronous.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorHalfSineAsynchronous);

	//Spec::SVariationFunctor variationFunctorHalfSineSingle;
	//variationFunctorHalfSineSingle.name = "HalfSineSingle";
	//variationFunctorHalfSineSingle.numberOfParents = 1;
	//variationFunctorHalfSineSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 10>;
	//variationFunctorHalfSineSingle.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::halfSineSingle);
	//variationFunctorHalfSineSingle.probability = 1.0;
	//variationFunctorHalfSineSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorHalfSineSingle);

	//Spec::SVariationFunctor variationFunctorHalfSineSynchronous;
	//variationFunctorHalfSineSynchronous.name = "HalfSineSynchronous";
	//variationFunctorHalfSineSynchronous.numberOfParents = 1;
	//variationFunctorHalfSineSynchronous.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 10>;
	//variationFunctorHalfSineSynchronous.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::halfSineSynchronous);
	//variationFunctorHalfSineSynchronous.probability = 1.0;
	//variationFunctorHalfSineSynchronous.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorHalfSineSynchronous);

	Spec::SVariationFunctor variationFunctorDeletionLInt;
	variationFunctorDeletionLInt.name = "DeletionLInt";
	variationFunctorDeletionLInt.numberOfParents = 1;
	variationFunctorDeletionLInt.parentSelectionFunction = std::bind_front(MGEA::metricProportionalN<1>, "gain");
	variationFunctorDeletionLInt.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariationWithGenotypeProxies, this, std::bind_front(&MGEA::deletionLInt, 2));
	variationFunctorDeletionLInt.probability = 1.0;
	variationFunctorDeletionLInt.removeParentsFromMatingPool = false;
	ea.registerVariationFunctor(variationFunctorDeletionLInt);

	Spec::SVariationFunctor variationFunctorDirectionalLInt;
	variationFunctorDirectionalLInt.name = "DirectionalLInt";
	variationFunctorDirectionalLInt.numberOfParents = 1;
	variationFunctorDirectionalLInt.parentSelectionFunction = std::bind_front(MGEA::metricProportionalN<1>, "gain");
	variationFunctorDirectionalLInt.variationFunctionFromIndividualPtrs = std::bind_front(&MotionGenerator::computeVariationWithIndividualPointers, this, std::bind_front(&MGEA::directionalLInt, 2));
	variationFunctorDirectionalLInt.probability = 1.0;
	variationFunctorDirectionalLInt.removeParentsFromMatingPool = false;
	ea.registerVariationFunctor(variationFunctorDirectionalLInt);

	Spec::SVariationFunctor variationFunctorSNVLInt;
	variationFunctorSNVLInt.name = "SNVLInt";
	variationFunctorSNVLInt.numberOfParents = 1;
	variationFunctorSNVLInt.parentSelectionFunction = std::bind_front(MGEA::metricProportionalN<1>, "fitness");
	variationFunctorSNVLInt.variationFunctionFromGenotypeProxies = std::bind_front(&MotionGenerator::computeVariationWithGenotypeProxies, this, std::bind_front(&MGEA::snvLInt, 2));
	variationFunctorSNVLInt.probability = 1.0;
	variationFunctorSNVLInt.removeParentsFromMatingPool = false;
	ea.registerVariationFunctor(variationFunctorSNVLInt);
}

Spec::MaybePhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->outputExists()) {
		return genPx;
	}
	auto maybeSimulationDataPtr = database.requestSimulationResult(simLogPtr->info()).get();
	if (std::unexpected(MGEA::ErrorCode::SimulationError) == maybeSimulationDataPtr) {
		return std::unexpected(DEvA::ErrorCode::InvalidTransform);
	}
	if (maybeSimulationDataPtr.has_value() and maybeSimulationDataPtr.value()) {
		if (!maybeSimulationDataPtr.value()->valid()) {
			return std::unexpected(DEvA::ErrorCode::InvalidTransform);
		}
		return genPx;
	}
	return std::unexpected(DEvA::ErrorCode::InvalidTransform);
}

Spec::MetricVariantMap MotionGenerator::evaluateIndividualFromGenotypeProxy(Spec::GenotypeProxy genPx) {
	//auto & timer = DTimer::simple("evaluate()").newSample().begin();
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->fitnessExists()) {
		//timer.end();
		return simLogPtr->data()->metrics;
	}
	SimulationDataPtr simDataPtr = simLogPtr->data();

	bool hasHeelHeight = simDataPtr->outputs.contains("heelHeight");
	bool hasToeHeight = simDataPtr->outputs.contains("toeHeight");
	bool hasFingertipHeight = simDataPtr->outputs.contains("fingertipHeight");
	bool hasPalmHeight = simDataPtr->outputs.contains("palmHeight");
	if (!hasToeHeight or !hasFingertipHeight or !hasHeelHeight or !hasPalmHeight) {
		spdlog::error("There is no position named \"heelHeight\" or \"toeHeight\" or \"fingertipHeight\" or \"palmHeight\" in simulation output {}", genPx);
		//timer.end();
		return {};
	}
	double timeStep = motionParameters.simStep;
	auto & fingertipHeight = simDataPtr->outputs.at("fingertipHeight");
	auto & palmHeight = simDataPtr->outputs.at("palmHeight");
	auto & heelHeight = simDataPtr->outputs.at("heelHeight");
	auto & toeHeight = simDataPtr->outputs.at("toeHeight");
	auto & comX = simDataPtr->outputs.at("centerOfMassX");
	auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
	auto & shoulderAngle = simDataPtr->outputs.at("shoulderAngle");
	bool sameSize = fingertipHeight.size() == toeHeight.size()
		and fingertipHeight.size() == heelHeight.size()
		and fingertipHeight.size() == palmHeight.size();
	if (!sameSize) {
		spdlog::error("\"heelHeight\", \"toeHeight\", \"palmHeight\" and \"fingertipHeight\" are not of same size");
		//timer.end();
		return {};
	}

	Spec::MetricVariantMap metrics;

	auto absLambda = [](double prev, double next) {
		return prev + std::abs(next);
	};
	auto onlyPositiveLambda = [](double prev, double next) {
		return prev + std::max(next, 0.0);
	};
	double fingertipHeightSum = std::accumulate(fingertipHeight.begin(), fingertipHeight.end(), 0.0, onlyPositiveLambda);
	double palmHeightSum = std::accumulate(palmHeight.begin(), palmHeight.end(), 0.0, onlyPositiveLambda);
	double heelHeightSum = std::accumulate(heelHeight.begin(), heelHeight.end(), 0.0, absLambda);
	double heelHeightMax = *std::max_element(heelHeight.begin(), heelHeight.end());
	double toeHeightSum = std::accumulate(toeHeight.begin(), toeHeight.end(), 0.0, absLambda);
	double toeHeightMax = *std::max_element(toeHeight.begin(), toeHeight.end());
	double comXSum = std::accumulate(comX.begin(), comX.end(), 0.0, absLambda);
	double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);
	double shoulderAngleDiffSum{};
	for (auto it(shoulderAngle.begin()); it != shoulderAngle.end() and std::next(it) != shoulderAngle.end(); ++it) {
		auto const & currentElement(*it);
		auto const & nextElement(*std::next(it));
		shoulderAngleDiffSum += std::min(currentElement - nextElement, 0.0);
	}
	//double fitness = diff * timeStep - std::abs(fingertipHeightSum);
	double fitness;
	//if (fingertipHeightSum <= 0.0 and palmHeightSum <= 0.0 and shoulderAngleDiffSum >= 0.0) {
	if (fingertipHeightSum <= 0.0 and palmHeightSum <= 0.0) {
		//fitness = std::min(toeHeightSum, heelHeightSum) * timeStep;
		//fitness = std::min(toeHeightSum, heelHeightSum) * timeStep;
		fitness = (2 * comZSum - comXSum) * timeStep / motionParameters.simStop();
		//fitness = toeHeightMax + heelHeightMax;
	} else {
		fitness = 0.0;
		if (fingertipHeightSum > 0.0) {
			fitness -= fingertipHeightSum;
		}
		if (palmHeightSum > 0.0) {
			fitness -= palmHeightSum;
		}
		//fitness += shoulderAngleDiffSum;
	}
	metrics["fingertipHeightSum"] = fingertipHeightSum;
	metrics["palmHeightSum"] = palmHeightSum;
	metrics["heelHeightSum"] = heelHeightSum;
	metrics["heelHeightMax"] = heelHeightMax;
	metrics["toeHeightSum"] = toeHeightSum;
	metrics["toeHeightMax"] = toeHeightMax;
	metrics["comXSum"] = comXSum;
	metrics["comZSum"] = comZSum;
	metrics["shoulderAngleDiffSum"] = shoulderAngleDiffSum;
	metrics["fitness"] = fitness;

	simLogPtr->data()->metrics = metrics;
	database.saveSimulationMetrics(simLogPtr->info(), metrics);
	//spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
	//timer.end();
	return metrics;
}

Spec::MetricVariantMap MotionGenerator::evaluateIndividualFromIndividualPtr(Spec::IndividualPtr iptr) {
	double maximumFitnessDiff(0.0);
	for (auto const& parent : iptr->parents) {
		auto const& parentFitness(std::get<double>(parent->metrics.at("fitness")));
		for (auto const& grandparent : parent->parents) {
			auto const& grandparentFitness(std::get<double>(grandparent->metrics.at("fitness")));
			double fitnessDiff(parentFitness - grandparentFitness);
			maximumFitnessDiff = std::max(maximumFitnessDiff, fitnessDiff);
		}
	}

	Spec::MetricVariantMap metrics;
	metrics["gain"] = maximumFitnessDiff;
	return metrics;
}

void MotionGenerator::survivorSelection(std::size_t count, Spec::IndividualPtrs & iptrs) {
	bool hasNonnegative = std::any_of(iptrs.begin(), iptrs.end(), [](auto & iptr) {
		return std::get<double>(iptr->metrics.at("fitness")) >= 0;
	});
	if (hasNonnegative) {
		auto it = std::remove_if(iptrs.begin(), iptrs.end(), [](auto & iptr) {
			return std::get<double>(iptr->metrics.at("fitness")) < 0;
		});
		iptrs.erase(it, iptrs.end());
	}

	std::vector<Spec::IndividualPtr> iptrVector(iptrs.begin(), iptrs.end());
	std::vector<std::size_t> iptrVectorIndices(iptrVector.size());
	std::iota(iptrVectorIndices.begin(), iptrVectorIndices.end(), 0);
	//std::vector<std::vector<double>> globalDistanceMatrix;
	std::vector<double> globalTotalDistanceVector;
	std::vector<double> localDistanceVector;

	//globalDistanceMatrix.resize(iptrVector.size());
	globalTotalDistanceVector.resize(iptrVector.size());
	localDistanceVector.resize(iptrVector.size());
	//for (std::size_t i(0); i < iptrVector.size(); ++i) {
	//	globalDistanceMatrix[i].resize(iptrVector.size(), 0.0);
	//}

	std::for_each(std::execution::par, iptrVectorIndices.begin(), iptrVectorIndices.end(), [&](std::size_t ind1) {
		double minLocalDistance(std::numeric_limits<double>::max());
		double totalDistance(0.0);
		for (auto ind2 : iptrVectorIndices) {
			if (ind1 == ind2) {
				continue;
			}
			auto & iptr1(iptrVector.at(ind1));
			auto & iptr2(iptrVector.at(ind2));
			auto simLogPtr1(database.getSimulationLog(iptr1->id));
			auto simLogPtr2(database.getSimulationLog(iptr2->id));
			if (!simLogPtr1 or !simLogPtr2) {
				throw std::logic_error("Simulation data should have been ready, but it's not...");
			}
			auto simDataPtr1(simLogPtr1->data());
			auto simDataPtr2(simLogPtr2->data());

			double distance(0.0);
			for (auto & jointName : motionParameters.jointNames) {
				auto & torqueData1(simDataPtr1->torque.at(jointName));
				auto & torqueData2(simDataPtr2->torque.at(jointName));
				for (std::size_t i(0); i != motionParameters.simSamples; ++i) {
					distance += std::pow(torqueData1.at(i) - torqueData2.at(i), 2);
				}
			}
			distance = std::sqrt(distance);

			//globalDistanceMatrix[ind1][ind2] = distance;
			//globalDistanceMatrix[ind2][ind1] = distance;
			minLocalDistance = std::min(minLocalDistance, distance);
			totalDistance += distance;
		}
		globalTotalDistanceVector[ind1] = totalDistance;
		localDistanceVector[ind1] = minLocalDistance;
	});

	std::vector<double> fitnesses(iptrVector.size());
	std::vector<double> novelties(iptrVector.size());
	std::vector<double> lucks(iptrVector.size());
	for (std::size_t i(0); i < iptrVector.size(); ++i) {
		auto & iptr(iptrVector.at(i));
		fitnesses.at(i) = std::get<double>(iptr->metrics.at("fitness"));
		novelties.at(i) = globalTotalDistanceVector[i];
		//novelties.at(i) = localDistanceVector[i];
		std::list<double> fitnessDiffs;
		std::transform(iptr->parents.begin(), iptr->parents.end(), std::back_inserter(fitnessDiffs), [&](auto& parent) {
			return std::get<double>(parent->metrics.at("fitness")) - std::get<double>(iptr->metrics.at("fitness"));
		});
		lucks.at(i) = 0.0;
		if (fitnessDiffs.size() > 0) {
			double luck = *std::min_element(fitnessDiffs.begin(), fitnessDiffs.end());
			lucks.at(i) = luck;
		}
	}

	auto normaliseLambda = [](std::vector<double> & v) {
		if (v.empty()) {
			return;
		}
		double maxValue(*std::max_element(v.begin(), v.end()));
		double minValue(*std::min_element(v.begin(), v.end()));
		if (maxValue == minValue) {
			v = std::vector<double>(v.size(), 1.0);
		} else {
			std::transform(v.begin(), v.end(), v.begin(), [&](auto f) { return (f - minValue) / (maxValue - minValue); });
		}
	};
	normaliseLambda(fitnesses);
	normaliseLambda(novelties);
	//std::transform(novelties.begin(), novelties.end(), novelties.begin(), [&](auto n) {
	//	return 1 - n;
	//});
	normaliseLambda(lucks);

	std::map<DEvA::IndividualIdentifier, double> valueMap;
	auto computeValueLambda = [&](std::size_t ind) {
		double fitnessSq(std::pow(fitnesses.at(ind), 2));
		double noveltySq(std::pow(novelties.at(ind), 2));
		double luckSq(std::pow(lucks.at(ind), 2));
		double value(std::sqrt(fitnessSq + noveltySq + luckSq));
		return value;
	};
	for (std::size_t i(0); i < iptrVector.size(); ++i) {
		double value(computeValueLambda(i));
		valueMap.emplace(std::make_pair(iptrVector.at(i)->id, value));
	}

	auto filterValueLambda = [&](auto & iptr) {
		auto & value(valueMap.at(iptr->id));
		return value < 1.0;
	};
	auto it = std::remove_if(iptrs.begin(), iptrs.end(), filterValueLambda);
	iptrs.erase(it, iptrs.end());

	//if (iptrs.size() > count) {
	//	iptrs.sort([&](auto & lhs, auto & rhs) {
	//		auto lhsId(lhs->id);
	//		auto rhsId(rhs->id);
	//		return valueMap.at(lhsId) > valueMap.at(rhsId);
	//	});

	//	iptrs.resize(count);
	//}

	iptrs.sort([&](auto& lhs, auto& rhs) {
		auto lhsFitness(std::get<double>(lhs->metrics.at("fitness")));
		auto rhsFitness(std::get<double>(rhs->metrics.at("fitness")));
		return lhsFitness > rhsFitness;
	});

	if (iptrs.size() > count) {
		iptrs.resize(count);
	}
}

void MotionGenerator::survivorSelectionPareto(Spec::IndividualPtrs & iptrs) {
	bool hasNonnegative = std::any_of(iptrs.begin(), iptrs.end(), [](auto& iptr) {
		return std::get<double>(iptr->metrics.at("fitness")) >= 0;
	});
	if (hasNonnegative) {
		auto it = std::remove_if(iptrs.begin(), iptrs.end(), [](auto& iptr) {
			return std::get<double>(iptr->metrics.at("fitness")) < 0;
		});
		iptrs.erase(it, iptrs.end());
	}

	std::vector<Spec::IndividualPtr> iptrVector(iptrs.begin(), iptrs.end());
	std::vector<std::size_t> iptrVectorIndices(iptrVector.size());
	std::iota(iptrVectorIndices.begin(), iptrVectorIndices.end(), 0);
	//std::vector<std::vector<double>> globalDistanceMatrix;
	std::vector<double> globalTotalDistanceVector;
	std::vector<double> localDistanceVector;

	//globalDistanceMatrix.resize(iptrVector.size());
	globalTotalDistanceVector.resize(iptrVector.size());
	localDistanceVector.resize(iptrVector.size());
	//for (std::size_t i(0); i < iptrVector.size(); ++i) {
	//	globalDistanceMatrix[i].resize(iptrVector.size(), 0.0);
	//}

	std::for_each(std::execution::par, iptrVectorIndices.begin(), iptrVectorIndices.end(), [&](std::size_t ind1) {
		double minLocalDistance(std::numeric_limits<double>::max());
		double totalDistance(0.0);
		for (auto ind2 : iptrVectorIndices) {
			if (ind1 == ind2) {
				continue;
			}
			auto& iptr1(iptrVector.at(ind1));
			auto& iptr2(iptrVector.at(ind2));
			auto simLogPtr1(database.getSimulationLog(iptr1->id));
			auto simLogPtr2(database.getSimulationLog(iptr2->id));
			if (!simLogPtr1 or !simLogPtr2) {
				throw std::logic_error("Simulation data should have been ready, but it's not...");
			}
			auto simDataPtr1(simLogPtr1->data());
			auto simDataPtr2(simLogPtr2->data());

			double distance(0.0);
			for (auto& jointName : motionParameters.jointNames) {
				auto& torqueData1(simDataPtr1->torque.at(jointName));
				auto& torqueData2(simDataPtr2->torque.at(jointName));
				for (std::size_t i(0); i != motionParameters.simSamples; ++i) {
					distance += std::pow(torqueData1.at(i) - torqueData2.at(i), 2);
				}
			}
			distance = std::sqrt(distance);

			//globalDistanceMatrix[ind1][ind2] = distance;
			//globalDistanceMatrix[ind2][ind1] = distance;
			minLocalDistance = std::min(minLocalDistance, distance);
			totalDistance += distance;
		}
		globalTotalDistanceVector[ind1] = totalDistance;
		localDistanceVector[ind1] = minLocalDistance;
	});

	std::vector<double> fitnesses(iptrVector.size());
	std::vector<double> novelties(iptrVector.size());
	std::vector<double> lucks(iptrVector.size());
	for (std::size_t i(0); i < iptrVector.size(); ++i) {
		auto& iptr(iptrVector.at(i));
		fitnesses.at(i) = std::get<double>(iptr->metrics.at("fitness"));
		novelties.at(i) = globalTotalDistanceVector[i];
		//novelties.at(i) = localDistanceVector[i];
		std::list<double> fitnessDiffs;
		std::transform(iptr->parents.begin(), iptr->parents.end(), std::back_inserter(fitnessDiffs), [&](auto& parent) {
			return std::get<double>(parent->metrics.at("fitness")) - std::get<double>(iptr->metrics.at("fitness"));
		});
		lucks.at(i) = 0.0;
		if (fitnessDiffs.size() > 0) {
			double luck = *std::min_element(fitnessDiffs.begin(), fitnessDiffs.end());
			lucks.at(i) = luck;
		}
	}

	auto normaliseLambda = [](std::vector<double>& v) {
		if (v.empty()) {
			return;
		}
		double maxValue(*std::max_element(v.begin(), v.end()));
		double minValue(*std::min_element(v.begin(), v.end()));
		if (maxValue == minValue) {
			v = std::vector<double>(v.size(), 1.0);
		}
		else {
			std::transform(v.begin(), v.end(), v.begin(), [&](auto f) { return (f - minValue) / (maxValue - minValue); });
		}
	};
	normaliseLambda(fitnesses);
	normaliseLambda(novelties);
	//std::transform(novelties.begin(), novelties.end(), novelties.begin(), [&](auto n) {
	//	return 1 - n;
	//});
	normaliseLambda(lucks);

	std::map<DEvA::IndividualIdentifier, double> valueMap;
	auto computeValueLambda = [&](std::size_t ind) {
		double fitnessSq(std::pow(fitnesses.at(ind), 2));
		double noveltySq(std::pow(novelties.at(ind), 2));
		double luckSq(std::pow(lucks.at(ind), 2));
		double value(std::sqrt(fitnessSq + noveltySq + luckSq));
		return value;
	};
	for (std::size_t i(0); i < iptrVector.size(); ++i) {
		double value(computeValueLambda(i));
		valueMap.emplace(std::make_pair(iptrVector.at(i)->id, value));
	}

	auto filterValueLambda = [&](auto& iptr) {
		auto& value(valueMap.at(iptr->id));
		return value < 1.0;
	};
	auto it = std::remove_if(iptrs.begin(), iptrs.end(), filterValueLambda);
	iptrs.erase(it, iptrs.end());

	iptrs.sort([&](auto& lhs, auto& rhs) {
		auto lhsFitness(std::get<double>(lhs->metrics.at("fitness")));
		auto rhsFitness(std::get<double>(rhs->metrics.at("fitness")));
		return lhsFitness > rhsFitness;
	});
}

Spec::Distance MotionGenerator::calculateTorqueDistance(DEvA::IndividualIdentifier id1, DEvA::IndividualIdentifier id2) {
	auto simLogPtr1(database.getSimulationLog(id1));
	auto simLogPtr2(database.getSimulationLog(id2));
	auto simDataPtr1(simLogPtr1->data());
	auto simDataPtr2(simLogPtr2->data());

	auto numSamples(motionParameters.simSamples);
	auto jointNames(motionParameters.jointNames);
	double distance(0);
	for (auto& jointName : jointNames) {
		auto& torque1(simDataPtr1->torque.at(jointName));
		auto& torque2(simDataPtr2->torque.at(jointName));
		for (std::size_t i(0); i != numSamples; ++i) {
			auto t1(torque1.at(i));
			auto t2(torque2.at(i));
			distance += std::abs(t1-t2);
		}
	}
	return distance;
}

Spec::Distance MotionGenerator::calculateTorqueSignDistance(DEvA::IndividualIdentifier id1, DEvA::IndividualIdentifier id2) {
	auto simLogPtr1(database.getSimulationLog(id1));
	auto simLogPtr2(database.getSimulationLog(id2));
	auto simDataPtr1(simLogPtr1->data());
	auto simDataPtr2(simLogPtr2->data());

	auto numSamples(motionParameters.simSamples);
	auto jointNames(motionParameters.jointNames);
	std::size_t distance(0);
	for (auto & jointName : jointNames) {
		auto & torque1(simDataPtr1->torque.at(jointName));
		auto & torque2(simDataPtr2->torque.at(jointName));
		for (std::size_t i(0); i != numSamples; ++i) {
			auto t1(torque1.at(i));
			auto t2(torque2.at(i));
			if (0.0 == t1 or 0.0 == t2) {
				continue;
			}
			if (t1 * t2 < 0.0) {
				++distance;
			}
		}
	}
	return distance;
}

Spec::Distance MotionGenerator::calculateAngleDistance(DEvA::IndividualIdentifier id1, DEvA::IndividualIdentifier id2) {
	auto simLogPtr1(database.getSimulationLog(id1));
	auto simLogPtr2(database.getSimulationLog(id2));
	auto simDataPtr1(simLogPtr1->data());
	auto simDataPtr2(simLogPtr2->data());

	auto numSamples(motionParameters.simSamples);
	std::vector<std::string> outputNames{};
	for (auto & outputPair : simDataPtr1->outputs) {
		auto& outputName(outputPair.first);
		if (!outputName.contains("Angle")) {
			continue;
		}
		outputNames.push_back(outputName);
	}
	std::size_t distance(0);
	for (auto & outputName : outputNames) {
		auto & angle1(simDataPtr1->outputs.at(outputName));
		auto & angle2(simDataPtr2->outputs.at(outputName));
		for (std::size_t i(0); i != numSamples; ++i) {
			auto t1(angle1.at(i));
			auto t2(angle2.at(i));
			if (0.0 == t1 or 0.0 == t2) {
				continue;
			}
			if (t1 * t2 < 0.0) {
				++distance;
			}
		}
	}
	return distance;
}

bool MotionGenerator::convergenceCheck(Spec::MetricVariantMap mVM) {
	//return f > 1.5 * motionParameters.simStop();
	return false;
}

void MotionGenerator::applyMotionParameters(SimulationDataPtr sptr) {
	sptr->time = motionParameters.time();
	sptr->params.emplace("simStart", motionParameters.simStart);
	sptr->params.emplace("simStop", motionParameters.simStop());
	sptr->params.emplace("simStep", motionParameters.simStep);
	sptr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
	sptr->alignment = motionParameters.alignment;
	sptr->timeout = motionParameters.timeout;
	sptr->contacts = motionParameters.contactParameters;
}

Spec::GenotypeProxies MotionGenerator::computeVariationWithGenotypeProxies(std::function<SimulationDataPtrs(MGEA::VariationParams, SimulationDataPtrs)> vFunc, Spec::GenotypeProxies parentProxies) {
	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	auto& timer = DTimer::simple("computeVariation()").newSample().begin();
	std::list<SimulationDataPtr> parentData;
	for (auto& parentProxy : parentProxies) {
		SimulationLogPtr parentLogPtr = database.getSimulationLog(parentProxy);
		SimulationDataPtr parentDataPtr = parentLogPtr->data();
		parentData.push_back(parentDataPtr);
	}

	MGEA::VariationParams variationParameters{
		.motionParameters = motionParameters,
		.pauseFlag = pauseFlag,
		.stopFlag = stopFlag
	};

	auto children = vFunc(variationParameters, parentData);

	Spec::GenotypeProxies childProxies;
	for (auto& child : children) {
		if (checkStopFlagAndMaybeWait()) {
			return {};
		}
		applyMotionParameters(child);
		auto childIndividualIdentifier = ea.reserveNewIndividualIdentifier();
		//auto childIndividualPtr = ea.createNewIndividual(childIndividualIdentifier);
		auto childLogPtr = database.registerSimulation(childIndividualIdentifier);
		// TODO Check if we could successfully create a new input?
		updateSimulationDataPtr({ .source = child, .target = childLogPtr->data() });
		MGEA::ErrorCode startError = database.startSimulation(childIndividualIdentifier);
		// TODO Check if start was successful?
		childProxies.push_back(childIndividualIdentifier);
	}
	timer.end();
	return childProxies;
}

Spec::GenotypeProxies MotionGenerator::computeVariationWithIndividualPointers(std::function<SimulationDataPtrs(MGEA::VariationParams, Spec::IndividualPtrs)> vFunc, Spec::IndividualPtrs parents) {
	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	auto& timer = DTimer::simple("computeVariation()").newSample().begin();

	for (auto & parent : parents) {
		SimulationLogPtr parentLogPtr = database.getSimulationLog(parent->genotypeProxy);
		SimulationDataPtr parentDataPtr = parentLogPtr->data();
		parent->genotype = parentDataPtr;
	}

	MGEA::VariationParams variationParameters{
		.motionParameters = motionParameters,
		.pauseFlag = pauseFlag,
		.stopFlag = stopFlag
	};

	auto children = vFunc(variationParameters, parents);

	Spec::GenotypeProxies childProxies;
	for (auto& child : children) {
		if (checkStopFlagAndMaybeWait()) {
			return {};
		}
		applyMotionParameters(child);
		auto childIndividualIdentifier = ea.reserveNewIndividualIdentifier();
		//auto childIndividualPtr = ea.createNewIndividual(childIndividualIdentifier);
		auto childLogPtr = database.registerSimulation(childIndividualIdentifier);
		// TODO Check if we could successfully create a new input?
		updateSimulationDataPtr({ .source = child, .target = childLogPtr->data() });
		MGEA::ErrorCode startError = database.startSimulation(childIndividualIdentifier);
		// TODO Check if start was successful?
		childProxies.push_back(childIndividualIdentifier);
	}
	timer.end();
	return childProxies;
}

Spec::GenotypeProxies MotionGenerator::computeGenesis(std::function<SimulationDataPtrs(MGEA::InitialiserParams)> gFunc) {
	MGEA::InitialiserParams initialiserParams{
		.motionParameters = motionParameters,
		.pauseFlag = pauseFlag,
		.stopFlag = stopFlag,
		.database = database
	};

	auto simDataPtrs(gFunc(initialiserParams));

	Spec::GenotypeProxies genotypeProxies;
	for (auto& simDataPtr : simDataPtrs) {
		applyMotionParameters(simDataPtr);
		auto individualIdentifier = ea.reserveNewIndividualIdentifier();
		auto logPtr = database.registerSimulation(individualIdentifier);
		*logPtr->data() = *simDataPtr;
		MGEA::ErrorCode startError = database.startSimulation(individualIdentifier);
		genotypeProxies.push_back(individualIdentifier);
	}

	return genotypeProxies;
}

void MotionGenerator::onEpochStart(std::size_t generation) {
	currentGeneration = generation;
	spdlog::info("Epoch {} started.", generation);
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	spdlog::info("Epoch {} ended.", generation);
	auto & lastGeneration = ea.genealogy.back();

	auto & timer = DTimer::simple("stats").newSample().begin();
	auto const & bestIndividualPtr = lastGeneration.front();
	database.saveVisualisationTarget(bestIndividualPtr->genotypeProxy);

	std::list<double> simulationTimes;
	for (auto & iptr : lastGeneration) {
		if (iptr->genotypeProxy.generation != generation) {
			continue;
		}
		auto simulationLogPtr = database.getSimulationLog(iptr->genotypeProxy);
		auto const & metadata = simulationLogPtr->data()->metadata;
		if (metadata.contains("totalTime")) {
			auto const & totalTime = metadata.at("totalTime");
			simulationTimes.push_back(totalTime);
		}
	}
	if (!simulationTimes.empty()) {
		auto minmax = std::minmax_element(simulationTimes.begin(), simulationTimes.end());
		double total = std::accumulate(simulationTimes.begin(), simulationTimes.end(), 0.0);
		double mean = total / static_cast<double>(simulationTimes.size());
		spdlog::info("(min, mean, max) simulation times were: ({:.3f}s, {:.3f}s, {:.3f}s)", *minmax.first, mean, *minmax.second);
		//spdlog::info("Total simulation time was: {:.3f}s", total);
		spdlog::info("Total simulation time was: {}", DTimer::printTime(static_cast<std::size_t>(total * 1000.0)));
	}
	timer.end();
	spdlog::info("\n{}", DTimer::print());
}
