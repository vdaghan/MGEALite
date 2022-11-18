#include "Common.h"
#include "MotionGeneration/Metrics/AngularVelocitySign.h"

#include <gtest/gtest.h>

TEST(AngularVelocitySign, ExtentChecks) {
	{
		MGEA::Angles angles{};
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::Angles angles{ {1.0, 0.0}, {2.0} };
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::Angles angles{ {1.0}, {2.0} };
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_TRUE(retVal.empty());
	}
	{
		MGEA::Angles angles{ {0.0, 0.0}, {1.0, 1.0} };
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_EQ(retVal.size(), 1);
		EXPECT_EQ(retVal[0].size(), 2);
	}
	{
		MGEA::Angles angles{ {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0} };
		auto retVal = MGEA::computeAngularVelocitySign(angles);
		EXPECT_EQ(retVal.size(), 1);
		EXPECT_EQ(retVal[0].size(), 2);
	}
}
