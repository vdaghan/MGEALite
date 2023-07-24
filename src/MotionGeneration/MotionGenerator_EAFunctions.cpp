// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/ParentSelectors/ParentSelectors.h"
#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"
#include "MotionGeneration/Variations/Variations.h"
#include "Wavelet/HaarWavelet.h"

#include <DTimer/DTimer.h>

#include <cmath>
#include <valarray>

void MotionGenerator::importMotionParameters(std::filesystem::path const & filename) {
	if (not std::filesystem::exists(filename)) {
		return;
	}
	std::ifstream f;
	f.open(filename, std::ios_base::in);
	if (not f.is_open()) {
		return;
	}
	JSON parseResult{};
	try {
		parseResult = JSON::parse(f);
		f.close();
	} catch (const std::exception &) {
		f.close();
		return;
	}

	if (not parseResult.contains("motionParameters")) {
		throw std::runtime_error("MotionGenerator::importMotionParameters: No motionParameters found in file " + filename.string());
	}
	auto & motionParametersJSON = parseResult.at("motionParameters");
	motionParameters.simStart = motionParametersJSON.at("simStart").get<double>();
	motionParameters.simStep = motionParametersJSON.at("simStep").get<double>();
	motionParameters.simSamples = motionParametersJSON.at("simSamples").get<std::size_t>();
	motionParameters.alignment = motionParametersJSON.at("alignment").get<int>();
	motionParameters.timeout = motionParametersJSON.at("timeout").get<double>();
	motionParameters.masses = motionParametersJSON.at("masses").get<std::map<std::string, double>>();
	motionParameters.jointNames = motionParametersJSON.at("jointNames").get<std::vector<std::string>>();
	motionParameters.jointLimits = motionParametersJSON.at("jointLimits").get<std::map<std::string, std::pair<double, double>>>();
	motionParameters.torqueSplineControlPointMinimumDistance = motionParametersJSON.at("torqueSplineControlPointMinimumDistance").get<std::size_t>();
	auto & contactParametersJSON = motionParametersJSON.at("contactParameters");
	ContactParameters contactParameters{};
	contactParameters.stiffness = contactParametersJSON.at("stiffness").get<double>();
	contactParameters.damping = contactParametersJSON.at("damping").get<double>();
	contactParameters.transitionRegionWidth = contactParametersJSON.at("transitionRegionWidth").get<double>();
	contactParameters.staticFriction = contactParametersJSON.at("staticFriction").get<double>();
	contactParameters.dynamicFriction = contactParametersJSON.at("dynamicFriction").get<double>();
	contactParameters.criticalVelocity = contactParametersJSON.at("criticalVelocity").get<double>();
	motionParameters.contactParameters = contactParameters;
}

void MotionGenerator::setupStandardFunctions() {
	functions.genesis.defineParametrised("MGEAGenesisBoundary", std::bind_front(MGEA::genesisBoundary, motionParameters), {});
	functions.genesis.defineParametrised("MGEAGenesisBoundaryWavelet", std::bind_front(MGEA::genesisBoundaryWavelet, motionParameters), {});
	functions.genesis.defineParametrised("MGEAGenesisImportFromFolderDefault", std::bind_front(MGEA::genesisImportFromFolder, motionParameters), {});
	functions.genesis.defineParametrisable("MGEAGenesisImportFromFolder", std::bind_front(MGEA::genesisImportFromFolder, motionParameters));
	functions.genesis.defineParametrisable("MGEAGenesisRandom", std::bind_front(MGEA::genesisRandom, motionParameters));
	functions.genesis.defineParametrised("MGEAGenesisZero", std::bind_front(MGEA::genesisZero, motionParameters), {});

	functions.parentSelection.defineParametrisable("MGEAParentSelectorMetricProportional", MGEA::metricProportional);

	functions.survivorSelection.defineParametrised("MGEASurvivorSelectorCullEquals", MGEA::cullEquals, {});
	functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorCullPartiallyDominated", MGEA::cullPartiallyDominated);
	functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorParetoFront", MGEA::paretoFront);
	//ea.functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorCullEquals", MGEA::survivorSelectionOverMetric);

	auto parametrisedVariationFromIPtrsLambda = [&](std::string functionName, auto function) {
		functions.variationFromIndividualPtrs.defineParametrised(functionName, std::bind_front(function, motionParameters), {});
	};
	auto parametrisableVariationFromIPtrsLambda = [&](std::string functionName, auto function) {
		functions.variationFromIndividualPtrs.defineParametrisable(functionName, std::bind_front(function, motionParameters));
	};

	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCrossoverAll", MGEA::crossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCrossoverSingle", MGEA::crossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCutAndCrossfillAll", MGEA::cutAndCrossfillAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCutAndCrossfillSingle", MGEA::cutAndCrossfillSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionAll", MGEA::deletionAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionSingle", MGEA::deletionSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsInsertionAll", MGEA::insertionAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsInsertionSingle", MGEA::insertionSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsKPointCrossoverAll", MGEA::kPointCrossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsKPointCrossoverSingle", MGEA::kPointCrossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsSNV", MGEA::snv);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsUniformCrossoverAll", MGEA::uniformCrossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsUniformCrossoverSingle", MGEA::uniformCrossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsWaveletSNV", MGEA::waveletSNV);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineAsynchronous", MGEA::halfSineAsynchronous);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineSingle", MGEA::halfSineSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineSynchronous", MGEA::halfSineSynchronous);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionLInt", MGEA::deletionLInt);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDirectionalLInt", MGEA::directionalLInt);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsSNVLInt", MGEA::snvLInt);

	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("angleDifferenceSum", &MGEA::angleDifferenceSum));
	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("angleDifferenceSumLinearWeighted", &MGEA::angleDifferenceSumLinearWeighted));
	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("averageOfAngleDifferenceSumsStepped", &MGEA::averageOfAngleDifferenceSumsStepped));

	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("maximumAngleDifference", &MGEA::maximumAngleDifference));
	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("maximumAngleDifferenceStepped", &MGEA::maximumAngleDifferenceStepped));
	metricFunctors.equivalences.emplace(std::pair("steppedDoubleEquivalence", &MGEA::steppedDoubleEquivalence));
	metricFunctors.orderings.emplace(std::pair("steppedDoubleLesser", &MGEA::steppedDoubleLesser));
	metricFunctors.orderings.emplace(std::pair("steppedDoubleGreater", &MGEA::steppedDoubleGreater));
	metricFunctors.metricToJSONObjectFunctions.emplace(std::pair("steppedDoubleConversion", &MGEA::steppedDoubleConversion));

	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("outputBetweenTwoValues", &MGEA::outputBetweenTwoValues));

	metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("angularVelocitySign", &MGEA::angularVelocitySign));
	metricFunctors.equivalences.emplace(std::pair("angularVelocitySignEquivalence", &MGEA::angularVelocitySignEquivalent));
	metricFunctors.metricToJSONObjectFunctions.emplace(std::pair("orderedVectorConversion", &MGEA::orderedVectorConversion));
}

Spec::MaybePhenotype MotionGenerator::transform(Spec::Genotype genotype) {
	return datastore.simulate(genotype);
}

//Spec::MetricVariantMap MotionGenerator::evaluateIndividualFromGenotypeProxy(Spec::GenotypeProxy genPx) {
//	//auto & timer = DTimer::simple("evaluate()").newSample().begin();
//	auto simLogPtr = database.getSimulationLog(genPx);
//	if (simLogPtr->fitnessExists()) {
//		//timer.end();
//		return simLogPtr->data()->metrics;
//	}
//	SimulationDataPtr simDataPtr = simLogPtr->data();
//
//	bool hasHeelHeight = simDataPtr->outputs.contains("heelZ");
//	bool hasToeHeight = simDataPtr->outputs.contains("toeZ");
//	bool hasFingertipHeight = simDataPtr->outputs.contains("fingertipZ");
//	bool hasPalmHeight = simDataPtr->outputs.contains("palmZ");
//	if (!hasToeHeight or !hasFingertipHeight or !hasHeelHeight or !hasPalmHeight) {
//		spdlog::error("There is no position named \"heelZ\" or \"toeZ\" or \"fingertipZ\" or \"palmZ\" in simulation output {}", genPx);
//		//timer.end();
//		return {};
//	}
//	double timeStep = motionParameters.simStep;
//	auto & fingertipZ = simDataPtr->outputs.at("fingertipZ");
//	auto & palmX = simDataPtr->outputs.at("palmX");
//	auto & palmZ = simDataPtr->outputs.at("palmZ");
//	auto & heelZ = simDataPtr->outputs.at("heelZ");
//	auto & toeZ = simDataPtr->outputs.at("toeZ");
//	auto & comX = simDataPtr->outputs.at("centerOfMassX");
//	auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
//	auto & shoulderAngle = simDataPtr->angles.at("shoulder");
//	bool sameSize = fingertipZ.size() == toeZ.size()
//		and fingertipZ.size() == heelZ.size()
//		and fingertipZ.size() == palmZ.size();
//	if (!sameSize) {
//		spdlog::error("\"heelZ\", \"toeZ\", \"palmZ\" and \"fingertipHeight\" are not of same size");
//		//timer.end();
//		return {};
//	}
//
//	Spec::MetricVariantMap metrics;
//
//	auto absLambda = [](double prev, double next) {
//		return prev + std::abs(next);
//	};
//	auto onlyPositiveLambda = [](double prev, double next) {
//		return prev + std::max(next, 0.0);
//	};
//	//double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0, onlyPositiveLambda);
//	//double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0, onlyPositiveLambda);
//	double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0);
//	double maxFingertipZ = *std::max_element(fingertipZ.begin(), fingertipZ.end());
//	double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0);
//	double maxPalmZ = *std::max_element(palmZ.begin(), palmZ.end());
//	double heelZSum = std::accumulate(heelZ.begin(), heelZ.end(), 0.0, absLambda);
//	double heelZMax = *std::max_element(heelZ.begin(), heelZ.end());
//	double toeZSum = std::accumulate(toeZ.begin(), toeZ.end(), 0.0, absLambda);
//	double toeZMax = *std::max_element(toeZ.begin(), toeZ.end());
//	double comXSum = std::accumulate(comX.begin(), comX.end(), 0.0, absLambda);
//	double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);
//	double shoulderAngleDiffSum{};
//	for (auto it(shoulderAngle.begin()); it != shoulderAngle.end() and std::next(it) != shoulderAngle.end(); ++it) {
//		auto const & currentElement(*it);
//		auto const & nextElement(*std::next(it));
//		shoulderAngleDiffSum += std::min(currentElement - nextElement, 0.0);
//	}
//	double balance = std::inner_product(comX.begin(), comX.end(), palmX.begin(), 0.0, std::plus<>(), [](auto const& cX, auto const& pX) {
//		return std::abs(cX - pX);
//	});
//	//double balance(0.0);
//	//for (std::size_t i(0); i != palmX.size(); ++i) {
//	//	balance += std::abs(comX[i] - palmX[i]);
//	//}
//	balance = balance * timeStep / motionParameters.simStop();
//	double fitness;
//	if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
//		fitness = comZSum * timeStep / motionParameters.simStop();
//	} else {
//		fitness = 0.0;
//		if (fingertipZSum > 0.0) {
//			fitness -= maxFingertipZ;
//		}
//		if (palmZSum > 0.0) {
//			fitness -= maxPalmZ;
//		}
//	}
//	metrics["fingertipZSum"] = fingertipZSum;
//	metrics["palmZSum"] = palmZSum;
//	metrics["heelZSum"] = heelZSum;
//	metrics["heelZMax"] = heelZMax;
//	metrics["toeZSum"] = toeZSum;
//	metrics["toeZMax"] = toeZMax;
//	metrics["comXSum"] = comXSum;
//	metrics["comZSum"] = comZSum;
//	metrics["shoulderAngleDiffSum"] = shoulderAngleDiffSum;
//	metrics["fitness"] = fitness;
//	metrics["balance"] = balance;
//	metrics["angularVelocitySign"] = MGEA::computeAngularVelocitySign(simLogPtr->data()->angles);
//
//	simLogPtr->data()->metrics = metrics;
//	database.saveSimulationMetrics(simLogPtr->info(), metrics);
//	//spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
//	//timer.end();
//	return metrics;
//}

void MotionGenerator::applyMotionParameters(SimulationDataPtr sptr) {
	sptr->time = motionParameters.time();
	sptr->alignment = motionParameters.alignment;
	sptr->timeout = motionParameters.timeout;
	sptr->params.emplace("simStart", motionParameters.simStart);
	sptr->params.emplace("simStop", motionParameters.simStop());
	sptr->params.emplace("simStep", motionParameters.simStep);
	sptr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
	sptr->masses = motionParameters.masses;
	sptr->contacts = motionParameters.contactParameters;
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	//auto & timer = DTimer::simple("stats").newSample().begin();

	auto const & bestIndividualPtr = genealogy.back().front();
	//database.saveVisualisationTarget(bestIndividualPtr->id);

	if (bestIndividual->genotype->torqueSplines) {
		for (auto & [jointName, torqueSpline] : bestIndividual->genotype->torqueSplines.value()) {
			auto & controlPoints(torqueSpline.controlPoints);
			spdlog::info("There are {} control points for {}: {}", controlPoints.size(), jointName, torqueSpline.str());
		}
	}

	//std::list<double> simulationTimes;
	//for (auto & iptr : lastGeneration) {
	//	if (iptr->genotypeProxy.generation != generation) {
	//		continue;
	//	}
	//	auto simulationLogPtr = database.getSimulationLog(iptr->genotypeProxy);
	//	auto const & metadata = simulationLogPtr->data()->metadata;
	//	if (metadata.contains("totalTime")) {
	//		auto const & totalTime = metadata.at("totalTime");
	//		simulationTimes.push_back(totalTime);
	//	}
	//}
	//if (!simulationTimes.empty()) {
	//	auto minmax = std::minmax_element(simulationTimes.begin(), simulationTimes.end());
	//	double total = std::accumulate(simulationTimes.begin(), simulationTimes.end(), 0.0);
	//	double mean = total / static_cast<double>(simulationTimes.size());
	//	spdlog::info("(min, mean, max) simulation times were: ({:.3f}s, {:.3f}s, {:.3f}s)", *minmax.first, mean, *minmax.second);
	//	//spdlog::info("Total simulation time was: {:.3f}s", total);
	//	spdlog::info("Total simulation time was: {}", DTimer::printTime(static_cast<std::size_t>(total * 1000.0)));
	//}

	//timer.end();
	//spdlog::info("\n{}", DTimer::print());
}
