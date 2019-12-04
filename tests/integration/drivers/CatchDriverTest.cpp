#include <cucumber-cpp/CucumberCppCatch.hpp>
#include <cucumber-cpp/autodetect.hpp>

#include "../../utils/DriverTestRunner.hpp"

using namespace cucumber;

THEN(SUCCEED_MATCHER) {
    ScenarioScope<SomeContext> ctx;
    REQUIRE(true);
}

THEN(FAIL_MATCHER) {
    ScenarioScope<SomeContext> ctx;
    REQUIRE(false);
}

THEN(PENDING_MATCHER_1) {
    pending();
}

THEN(PENDING_MATCHER_2) {
    pending(PENDING_DESCRIPTION);
}

using namespace cucumber::internal;

class CatchStepDouble : public CatchStep {
public:
    const InvokeResult invokeStepBody() {
        return CatchStep::invokeStepBody();
    };

    void body(){};

    static bool isInitialized() {
        return CatchStep::isInitialized();
    }
};

class CatchDriverTest : public DriverTest {
public:
    virtual void runAllTests() {
        stepInvocationInitsCatchTest();
        DriverTest::runAllTests();
    }

private:
    void stepInvocationInitsCatchTest() {
        std::cout << "= Init =" << std::endl;
        CatchStepDouble step;
        expectFalse("Framework is not initialized before the first test",
                    CatchStepDouble::isInitialized());
        step.invokeStepBody();
        expectTrue("Framework is initialized after the first test",
                   CatchStepDouble::isInitialized());
    }
};

int main() {
    CatchDriverTest test;
    return test.run();
}
