#ifndef CUKE_CATCHWRAPPER_HPP_
#define CUKE_CATCHWRAPPER_HPP_

#include <cucumber-cpp/internal/CukeExport.hpp>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC visibility push(default)
#endif


// needed to capture output of catch
#define CATCH_CONFIG_NOSTDOUT

#include "catch2/catch.hpp"

/* These interfere with cucumber-cpp defines. */
#undef GIVEN
#undef WHEN
#undef THEN

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC visibility pop
#endif

#endif // CUKE_CATCHWRAPPER_HPP_
