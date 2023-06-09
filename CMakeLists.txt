cmake_minimum_required(VERSION 3.0)

set(CMAKE_OSX_ARCHITECTURES "arm64")

project(gravity_sim)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

# Include directories with full paths instead of relative paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add compiler flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=armv8.3-a -march=native -flto")

# Enable link-time optimization (LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Set policy to enforce INTERPROCEDURAL_OPTIMIZATION
cmake_policy(SET CMP0069 NEW)

add_executable(gravity_sim ${SOURCES})

find_package(SFML COMPONENTS graphics window system)
target_link_libraries(gravity_sim sfml-graphics sfml-window sfml-system)

target_compile_features(gravity_sim PUBLIC cxx_std_20)

add_custom_target(run
    COMMAND gravity_sim
    DEPENDS gravity_sim
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
