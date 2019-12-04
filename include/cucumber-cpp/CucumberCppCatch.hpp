#ifndef CUKE_CATCHWRAPPER_HPP_
#define CUKE_CATCHWRAPPER_HPP_

// needed to capture output of catch
#define CATCH_CONFIG_NOSTDOUT

#include "catch2/catch.hpp"

/* These interfere with cucumber-cpp defines. */
#undef GIVEN
#undef WHEN
#undef THEN

#endif // CUKE_CATCHWRAPPER_HPP_
