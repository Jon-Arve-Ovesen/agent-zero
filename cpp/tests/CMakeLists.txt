cmake_minimum_required(VERSION 3.15)

# Download and configure GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable
add_executable(agent_zero_tests
    test_agent_core.cpp
    test_memory_manager.cpp
    test_task_executor.cpp
)

target_link_libraries(agent_zero_tests
    PRIVATE agent_zero_core
    PRIVATE GTest::gtest_main
)

# Set output directories for multi-config generators (Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
    set_target_properties(agent_zero_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    )
endif()

include(GoogleTest)
gtest_discover_tests(agent_zero_tests)
