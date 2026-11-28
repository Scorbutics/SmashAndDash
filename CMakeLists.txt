cmake_minimum_required(VERSION 3.9)
project(smash_and_dash)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SKA_BUILD_NO_TEST TRUE)

# ccache, when installed (the engine and the game : a file compiled before is taken from its cache)
include(external/ska/cmake/SkaCompilerCache.cmake)
# The web build (Emscripten) : tools/web/build.sh
include(external/ska/cmake/SkaWeb.cmake)

#External
set(curdir "external/ska")
FILE(GLOB children "${curdir}/ska_*")
FOREACH(child ${children})
	IF(IS_DIRECTORY ${child})
		MESSAGE("including engine directory -- " ${child}/src/src)
		include_directories(${child}/src/src)
	ENDIF()
ENDFOREACH()

FILE(GLOB children "${curdir}/external/*")
FOREACH(child ${children})
	IF(IS_DIRECTORY ${child})
		IF(EXISTS ${child}/src/src)
			MESSAGE("including engine external directory -- " ${child}/src/src)
			include_directories(${child}/src/src)
		ELSE()
			IF(EXISTS ${child}/include)
				MESSAGE("including engine external directory -- " ${child}/include)
				include_directories(${child}/include)
			ELSE()
				MESSAGE("including engine external directory -- " ${child})
				include_directories(${child})
			ENDIF()			
		ENDIF()
	ENDIF()
ENDFOREACH()

add_subdirectory(${curdir})

#Internal (Game code)
# Headless script tests (ctest) : see src/CMakeLists.txt
enable_testing()
add_subdirectory(src)

# Shaders (src/shaders/*.frag), compiled for the renderer in Shaders/ : loaded as "./Shaders/<name>"
file(GLOB SAD_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag)
ska_compile_shaders(smash_and_dash_shaders OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Shaders SOURCES ${SAD_SHADERS})
if(EMSCRIPTEN)
	# Packed in the page (src/CMakeLists.txt ASSETS) : made before it
	add_dependencies(smash_and_dash_web smash_and_dash_shaders)
endif()
