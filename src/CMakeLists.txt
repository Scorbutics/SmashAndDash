cmake_minimum_required(VERSION 3.9)
project(smash_and_dash)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_executable(smash_and_dash ${SOURCES})
set(TARGET_LIBS skalog skabase ska_ai ska_audio ska_core ska_draw ska_gui ska_particle ska_physic_chipmunk ska_script ska_world)
target_link_libraries(smash_and_dash ${TARGET_LIBS})
