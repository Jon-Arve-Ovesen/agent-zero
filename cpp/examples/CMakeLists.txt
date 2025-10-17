cmake_minimum_required(VERSION 3.15)

add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE agent_zero_core)
add_dependencies(basic_usage agent_zero_core)

add_executable(multi_paradigm multi_paradigm.cpp)
target_link_libraries(multi_paradigm PRIVATE agent_zero_core)
add_dependencies(multi_paradigm agent_zero_core)

add_executable(task_executor_example task_executor_example.cpp)
target_link_libraries(task_executor_example PRIVATE agent_zero_core)
add_dependencies(task_executor_example agent_zero_core)

# Set output directories for multi-config generators (Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
    set_target_properties(basic_usage multi_paradigm task_executor_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    )
endif()
