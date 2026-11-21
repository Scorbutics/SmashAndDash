cmake_minimum_required(VERSION 3.9)
project(smash_and_dash)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

include_directories(src)

# The game is a library ("libGame") loaded and run by the ska launcher executable (ska_main)
add_library(smash_and_dash SHARED ${SOURCES})
set_target_properties(smash_and_dash PROPERTIES OUTPUT_NAME Game)
add_dependencies(smash_and_dash ska_main)
set(TARGET_LIBS skalog skabase ska_ai ska_audio ska_core ska_draw ska_gui ska_particle ska_physic_chipmunk ska_script ska_world)
target_link_libraries(smash_and_dash ${TARGET_LIBS})
