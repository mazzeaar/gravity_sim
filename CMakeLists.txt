cmake_minimum_required(VERSION 3.0)

project(gravity_sim)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(gravity_sim ${SOURCES})

find_package(SFML COMPONENTS graphics window system)
target_link_libraries(gravity_sim sfml-graphics sfml-window sfml-system)

target_compile_features(gravity_sim PUBLIC cxx_std_20 )

add_custom_target(run
    COMMAND gravity_sim
    DEPENDS gravity_sim
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)