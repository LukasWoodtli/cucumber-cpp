// this define and include need to be first!
#define CATCH_CONFIG_RUNNER
#include "cucumber-cpp/CucumberCppCatch.hpp"

#include "cucumber-cpp/internal/drivers/CatchDriver.hpp"

#include <sstream>
#include <cassert>
#include <functional>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/once.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/static_assert.hpp>

namespace {
std::stringstream catchOutStream;
}
namespace Catch {

std::ostream& cout() {
    return catchOutStream;
}
std::ostream& clog() {
    return catchOutStream;
}
std::ostream& cerr() {
    return catchOutStream;
}
} // namespace Catch

namespace {

boost::function<void()> currentTestBody;

void exec_test_body() {
    assert(currentTestBody);
    if (currentTestBody) {
        currentTestBody();
    }
}

const char TEST_CASE_NAME[] = "catch-test-case-for-cucumber-cpp";

REGISTER_TEST_CASE(&exec_test_body, TEST_CASE_NAME);

Catch::Session catchSession;

const char TEST_APPLICATION_NAME[] = "cucumber-cpp-catch-app";
bool INITIALIZED = false;
void initSession() {
    static const char* args[] = {TEST_APPLICATION_NAME, "-w", "NoTests", TEST_CASE_NAME};
    static const size_t argsSize = sizeof(args) / sizeof(*args);
    BOOST_STATIC_ASSERT(argsSize == 4);
    int returnCode = catchSession.applyCommandLine(argsSize, args);
    assert(returnCode == 0);
    (void)returnCode;
    INITIALIZED = true;
}

const char CATCH_OUTPUT_SEPARATOR[] = "======================";

std::string filterUnNeededTestOutput(const std::string& input) {
    std::vector<std::string> lines;
    boost::split(lines, input, boost::is_any_of("\r\n"), boost::token_compress_on);
    std::stringstream output;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        if (!boost::algorithm::contains(*it, CATCH_OUTPUT_SEPARATOR)
            && !boost::starts_with(*it, "test cases: ")
            && !boost::starts_with(*it, "test cases: ")
            && !boost::starts_with(*it, "assertions: ")
            && !boost::algorithm::contains(*it, "Filters: ")
            && !boost::algorithm::contains(*it, "All tests passed")
            && !boost::algorithm::contains(*it, TEST_APPLICATION_NAME)
            && !boost::algorithm::contains(*it, "Run with -? for options")) {
            output << *it << '\n';
        }
    }
    return output.str();
}

} // namespace

namespace cucumber {
namespace internal {

InvokeResult getResult(const int numberFailedTests) {
    if (numberFailedTests > 0) {
        const auto output = filterUnNeededTestOutput(catchOutStream.str());
        catchOutStream.str("");
        return InvokeResult::failure(output);
    }
    return InvokeResult::success();
}

const InvokeResult CatchStep::invokeStepBody() {
    static boost::once_flag initialized;
    boost::call_once(initialized, initSession);

    currentTestBody = boost::bind(&CatchStep::body, this);
    int numFailed = catchSession.run();
    currentTestBody.clear();

    return getResult(numFailed);
}

// only used for testing
bool CatchStep::isInitialized() {
    return INITIALIZED;
}

} // namespace internal
} // namespace cucumber
