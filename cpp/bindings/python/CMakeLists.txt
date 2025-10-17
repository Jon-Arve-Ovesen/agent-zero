cmake_minimum_required(VERSION 3.15)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Try to find pybind11
find_package(pybind11 CONFIG QUIET)

if(NOT pybind11_FOUND)
    message(STATUS "pybind11 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Python bindings module
pybind11_add_module(agent_zero_native MODULE
    agent_zero_bindings.cpp
)

target_link_libraries(agent_zero_native PRIVATE agent_zero_core)

# Ensure agent_zero_core is built before Python bindings
add_dependencies(agent_zero_native agent_zero_core)

# Installation
install(TARGETS agent_zero_native
    LIBRARY DESTINATION ${Python3_SITELIB}
)
