#ifndef CUKE_CATCHWRAPPER_HPP_
#define CUKE_CATCHWRAPPER_HPP_

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif // _MSC_VER


// needed to capture output of catch
#define CATCH_CONFIG_NOSTDOUT

#include "catch2/catch.hpp"

/* These interfere with cucumber-cpp defines. */
#undef GIVEN
#undef WHEN
#undef THEN

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif // _MSC_VER

#endif // CUKE_CATCHWRAPPER_HPP_
