#include <cucumber-cpp/CucumberCppCatch.hpp>

#define EXPECT_EQ(a, b) REQUIRE((a) == (b))

#include "CalculatorSteps.cpp"
