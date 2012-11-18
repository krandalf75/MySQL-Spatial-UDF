cmake_minimum_required (VERSION 2.6)
project (MySpatialUDF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
set(CMAKE_COLOR_MAKEFILE ON)

#diagnosing info
message(STATUS "c++ compiler ... " ${CMAKE_CXX_COMPILER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    set(LIB_INSTALL_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/)
elseif(UNIX)
    set(LIB_INSTALL_PATH lib/)
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# needed packages
find_package(PROJ REQUIRED)
if(PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    link_directories(${PROJ_LIBRARY})
endif(PROJ_FOUND)

find_package(GEOS REQUIRED)
if(GEOS_FOUND)
    include_directories(${GEOS_INCLUDE_DIR})
    link_directories(${GEOS_LIBRARY})
endif(GEOS_FOUND)

find_package(MYSQL REQUIRED)
if (MYSQL_FOUND)
    include_directories(${MYSQL_INCLUDE_DIR})
    link_directories(${MYSQL_LIBRARY})
endif(MYSQL_FOUND)


if(MSVC)
  option(BUILD_RELEASE_DEBUG_DIRS "Set OFF to build each configuration to a separate directory" OFF)
  mark_as_advanced(BUILD_RELEASE_DEBUG_DIRS)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
	

# For windows, do not allow the compiler to use default target (Vista).
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501 -D_USRDLL)
endif(WIN32)

file(GLOB MSUDF_SRC
    "*.cpp"
)

ADD_LIBRARY(MSUDF SHARED ${MSUDF_SRC})
SET_TARGET_PROPERTIES(MSUDF PROPERTIES OUTPUT_NAME "MySpatialUDF")

#ADD_LIBRARY(MSUDF-static STATIC ${MSUDF_SRC})
#SET_TARGET_PROPERTIES(MSUDF-static PROPERTIES OUTPUT_NAME "MySpatialUDF")
#SET_TARGET_PROPERTIES(MSUDF-static PROPERTIES PREFIX "lib")

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_MBCS)
endif(MSVC)


#/D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_USRDLL" /D "_WINDLL" /D "_MBCS"-DCHANGE_BY_BISHOP
    
add_definitions(-DSTRICT)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(WIN32)
  # Postfix of DLLs:
  set(LIB_DEBUG_POSTFIX d)
else()
  # Postfix of so's:
  set(LIB_DEBUG_POSTFIX)
endif()

if(PROJ_FOUND)
    target_link_libraries(MSUDF ${PROJ_LIBRARY})
    # target_link_libraries(MSUDF-static  ${PROJ_LIBRARY})
endif(PROJ_FOUND)
if(GEOS_FOUND)
    target_link_libraries(MSUDF ${GEOS_LIBRARY})
    # target_link_libraries(MSUDF-static  ${PROJ_LIBRARY})
endif(GEOS_FOUND)

if (MYSQL_PLUGIN_DIR)
	INSTALL(TARGETS MSUDF DESTINATION ${MYSQL_PLUGIN_DIR})
endif(MYSQL_PLUGIN_DIR)

configure_file (
  "${PROJECT_SOURCE_DIR}/MySpatialUDF.sql.in"
  "${PROJECT_SOURCE_DIR}/MySpatialUDF.sql"
  )

