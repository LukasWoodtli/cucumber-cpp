
cmake_minimum_required(VERSION 3.11)

include(FetchContent)


FetchContent_Declare(gherkin-c
        GIT_REPOSITORY https://github.com/cucumber/gherkin-c
        )

FetchContent_GetProperties(gherkin-c)
if(NOT gherkin-c_POPULATED)
    FetchContent_Populate(gherkin-c)
    add_subdirectory(${gherkin-c_SOURCE_DIR} ${gherkin-c_BINARY_DIR} ${gherkin-c_DIR})
endif()

add_library(gherkin::gherkin ALIAS gherkin)


