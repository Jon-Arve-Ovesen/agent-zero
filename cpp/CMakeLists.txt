cmake_minimum_required(VERSION 3.15)
project(AgentZero VERSION 0.9.6 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_NODE_BINDINGS "Build Node.js bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directories for multi-config generators (Visual Studio)
# This must be set before any targets are created
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
endif()

# Include directories
include_directories(include)

# Source files for the core library
set(CORE_SOURCES
    src/agent_core.cpp
    src/task_executor.cpp
    src/memory_manager.cpp
    src/tool_interface.cpp
    src/multi_lang_bridge.cpp
)

# Core library
add_library(agent_zero_core ${CORE_SOURCES})
target_include_directories(agent_zero_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable modern C++ features
target_compile_features(agent_zero_core PUBLIC cxx_std_17)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(agent_zero_core PRIVATE AGENT_ZERO_WINDOWS)
elseif(APPLE)
    target_compile_definitions(agent_zero_core PRIVATE AGENT_ZERO_MACOS)
else()
    target_compile_definitions(agent_zero_core PRIVATE AGENT_ZERO_LINUX)
endif()

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(bindings/python)
endif()

# Node.js bindings
if(BUILD_NODE_BINDINGS)
    # Node.js bindings are built separately with node-gyp
    message(STATUS "Node.js bindings will be built with node-gyp")
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation rules
install(TARGETS agent_zero_core
    EXPORT AgentZeroTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/agent_zero
    DESTINATION include
)

# Export targets
install(EXPORT AgentZeroTargets
    FILE AgentZeroTargets.cmake
    NAMESPACE AgentZero::
    DESTINATION lib/cmake/AgentZero
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    AgentZeroConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AgentZeroConfigVersion.cmake
    DESTINATION lib/cmake/AgentZero
)
