// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs directionalLInt(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		std::size_t shift(parameters.at("shift").get<std::size_t>());

		auto & parent(*iptrs.front());
		auto & parentGenotype = parent.genotype;

		if (parent.parents.empty()) {
			return {};
		}
		double grandparentFitness(std::numeric_limits<double>::lowest());
		Spec::Genotype grandparentGenotype{};

		for (auto & grandparent : parent.parents) {
			if (grandparent->metricMap.at("fitness").as<double>() > grandparentFitness) {
				grandparentGenotype = grandparent->genotype;
				grandparentFitness = grandparent->metricMap.at("fitness").as<double>();
			}
		}

		// TODO: This check is embarrassing
		if (!grandparentGenotype) {
			return {};
		}

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();
		childDataPtr->time = parentGenotype->time;
		childDataPtr->params = parentGenotype->params;
		childDataPtr->torque = parentGenotype->torque;

		std::size_t const simLength = motionParameters.simSamples;
		std::size_t const numJoints = motionParameters.jointNames.size();
		decltype(parentGenotype->torque) parentTorqueMap(parentGenotype->torque);
		decltype(parentGenotype->torque) grandparentTorqueMap(grandparentGenotype->torque);
		decltype(parentGenotype->torque) differenceTorqueMap(grandparentGenotype->torque);
		double maxMultiplier(0.0);
		for (auto & jointName : motionParameters.jointNames) {
			auto const & parentTorqueVector(parentTorqueMap.at(jointName));
			auto const & grandparentTorqueVector(grandparentTorqueMap.at(jointName));
			std::pair<double, double> const & jointLimits = motionParameters.jointLimits.at(jointName);
			for (std::size_t i(0); i != simLength; ++i) {
				auto const & parentTorque(parentTorqueVector[i]);
				auto const & grandparentTorque(grandparentTorqueVector[i]);
				double diff(parentTorque - grandparentTorque);
				if (0.0 == diff) {
					continue;
				}
				double maxMultiplierHere(0.0);
				if (diff >= 0.0) {
					maxMultiplierHere = (jointLimits.second - grandparentTorque) / diff;
				} else {
					maxMultiplierHere = (grandparentTorque - jointLimits.first) / diff;
				}
				if (maxMultiplierHere > maxMultiplier) {
					maxMultiplier = maxMultiplierHere;
				}
			}
		}

		double const randMultiplier = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(0.0, maxMultiplier);

		for (auto& jointName : motionParameters.jointNames) {
			auto const & parentTorqueVector(parentTorqueMap.at(jointName));
			auto const & grandparentTorqueVector(grandparentTorqueMap.at(jointName));
			auto & childTorqueVector(childDataPtr->torque.at(jointName));
			for (std::size_t i(0); i != simLength; ++i) {
				auto const & parentTorque(parentTorqueVector[i]);
				auto const & grandparentTorque(grandparentTorqueVector[i]);
				double diff(parentTorque - grandparentTorque);
				childTorqueVector[i] = grandparentTorque + diff * randMultiplier;
			}
		}

		return { childDataPtr };
	}
}
