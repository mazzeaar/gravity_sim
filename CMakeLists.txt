cmake_minimum_required(VERSION 3.0)

project(gravity_sim)

# cpp files are in src and h files are in include
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

# Add the executable with the list of source files
add_executable(gravity_sim ${SOURCES})

find_package(SFML COMPONENTS graphics window system)

set(SDL2_INCLUDE_DIR /opt/homebrew/Cellar/sdl2/2.26.5/include/SDL2)
set(SDL2_LIBRARY /opt/homebrew/Cellar/sdl2/2.26.5/lib/libSDL2.dylib)

include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(gravity_sim ${SDL2_LIBRARY})

# Link SFML
target_link_libraries(gravity_sim sfml-graphics sfml-window sfml-system)

# Set C++20

target_compile_features(gravity_sim PUBLIC cxx_std_20)

# add a run command to the makefile such that we compile, run and then delete the executable
add_custom_target(run
    COMMAND gravity_sim
    DEPENDS gravity_sim
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)