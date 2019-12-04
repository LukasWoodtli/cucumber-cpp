#ifndef CUKE_CATCHWRAPPER_HPP_
#define CUKE_CATCHWRAPPER_HPP_

#pragma GCC visibility push(default)

// needed to capture output of catch
#define CATCH_CONFIG_NOSTDOUT

#include "catch2/catch.hpp"

/* These interfere with cucumber-cpp defines. */
#undef GIVEN
#undef WHEN
#undef THEN

#pragma GCC visibility pop
#endif // CUKE_CATCHWRAPPER_HPP_
