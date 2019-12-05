#ifndef CUKE_CATCHDRIVER_H_
#define CUKE_CATCHDRIVER_H_

#include <cucumber-cpp/internal/CukeExport.hpp>
#include "../step/StepManager.hpp"

#include <catch2/catch.hpp>

namespace Catch {
CUCUMBER_CPP_EXPORT std::ostream& cout();
CUCUMBER_CPP_EXPORT std::ostream& clog();
CUCUMBER_CPP_EXPORT std::ostream& cerr();
} // namespace Catch

namespace cucumber {
namespace internal {

class CUCUMBER_CPP_EXPORT CatchStep : public BasicStep {
protected:
    const InvokeResult invokeStepBody();

    static bool isInitialized();
};

} // namespace internal
} // namespace cucumber

#define STEP_INHERITANCE(step_name) ::cucumber::internal::CatchStep
#endif // CUKE_CATCHDRIVER_H_
