cmake_minimum_required(VERSION 3.10)
project(EstoDistributions-PI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

# list source files
file(GLOB SOURCES "src/*.cpp")
add_executable(algo_exec ${SOURCES})
target_include_directories(algo_exec PUBLIC include)
# Add tests (if using a framework like GTest or Catch2)
# target_link_libraries(algo_exec private Catch2::Catch2WithMain)
