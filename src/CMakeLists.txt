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
set(TARGET_LIBS skalog skabase ska_ai ska_audio ska_core ska_draw ska_gui ska_particle ska_physic_chipmunk ska_script ska_scripting ska_world)
target_link_libraries(smash_and_dash ${TARGET_LIBS})

# The game bindings script templates are deployed with the skalang standard library ones (in "std", next to the executables)
add_custom_command(TARGET smash_and_dash POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/std
	COMMENT "Deploying the game script templates")

# Static check of the scripts (skalang_check_scripts) : the build fails when a script does not compile
# (syntax, types, calls of the engine and game functions, declared by their templates)
set(GAME_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
# Scripts only imported by the other scripts (no "run" function)
set(GAME_SCRIPT_MODULES
	${GAME_ROOT_DIR}/Levels/dialog.miniska
	${GAME_ROOT_DIR}/Levels/story.miniska
	${GAME_ROOT_DIR}/Levels/surf.miniska
	${GAME_ROOT_DIR}/Levels/jump_wall.miniska
	# Called by the battles (BattleRules)
	${GAME_ROOT_DIR}/Data/Battle/rules.miniska
	${GAME_ROOT_DIR}/Data/Battle/ai.miniska)
file(GLOB_RECURSE GAME_SCRIPTS CONFIGURE_DEPENDS ${GAME_ROOT_DIR}/Levels/*.miniska ${GAME_ROOT_DIR}/Resources/*.miniska)
list(REMOVE_ITEM GAME_SCRIPTS ${GAME_SCRIPT_MODULES})
skalang_check_scripts(smash_and_dash_scripts_check
	SCRIPTS ${GAME_SCRIPTS}
	MODULES ${GAME_SCRIPT_MODULES}
	BIND_DIRECTORIES ${SKA_SCRIPT_TEMPLATES_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/scripts
	WORKING_DIRECTORY ${GAME_ROOT_DIR})

# Headless script tests (ctest, no window) : the scripts run with the templates of the engine and game functions (their
# default bodies), and the fakes of tests/fakes (ex : an engine keeping the game variables)
file(GLOB GAME_SCRIPT_TESTS CONFIGURE_DEPENDS ${GAME_ROOT_DIR}/tests/*_test.miniska)
skalang_test_scripts(game_script
	TESTS ${GAME_SCRIPT_TESTS}
	BIND_DIRECTORIES ${SKA_SCRIPT_TEMPLATES_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${GAME_ROOT_DIR}/tests/fakes
	WORKING_DIRECTORY ${GAME_ROOT_DIR})
