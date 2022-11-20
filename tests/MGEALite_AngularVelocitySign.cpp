#include "Common.h"
#include "MotionGeneration/Metrics/Metrics.h"

#include <gtest/gtest.h>

TEST(AngularVelocitySign, ExtentChecks) {
	{
		MGEA::VectorMap angles{};
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::VectorMap angles{};
		angles.emplace(std::make_pair("1", MGEA::DataVector{ 1.0, 0.0 }));
		angles.emplace(std::make_pair("2", MGEA::DataVector{ 2.0 }));
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::VectorMap angles{};
		angles.emplace(std::make_pair("1", MGEA::DataVector{ 1.0 }));
		angles.emplace(std::make_pair("2", MGEA::DataVector{ 2.0 }));
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::VectorMap angles{};
		angles.emplace(std::make_pair("1", MGEA::DataVector{ 0.0, 0.0 }));
		angles.emplace(std::make_pair("2", MGEA::DataVector{ 1.0, 1.0 }));
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_EQ(retVal.size(), 1);
		EXPECT_EQ(retVal[0].size(), 2);
	}
	{
		MGEA::VectorMap angles{};
		angles.emplace(std::make_pair("1", MGEA::DataVector{ 0.0, 0.0, 0.0 }));
		angles.emplace(std::make_pair("2", MGEA::DataVector{ 1.0, 1.0, 1.0 }));
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_EQ(retVal.size(), 1);
		EXPECT_EQ(retVal[0].size(), 2);
	}
}
