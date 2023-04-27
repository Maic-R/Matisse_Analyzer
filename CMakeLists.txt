#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(Matisse_analysis) #FIXME

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
#option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
#if(WITH_GEANT4_UIVIS)
#  find_package(Geant4 REQUIRED ui_all vis_all)
#else()
#  find_package(Geant4 REQUIRED)
#endif()
find_package(ROOT REQUIRED COMPONENTS RIO Net)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${ROOT_LIBRARY_DIR})



#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
#include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
# find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
		            ${ROOT_INCLUDE_DIRS})
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h 
                  ${PROJECT_SOURCE_DIR}/include/*.hpp)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(Matisse_analysis main.cpp ${sources} ${headers})
target_link_libraries(Matisse_analysis ${ROOT_LIBRARIES})
# ${} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build hadrontherapy. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#file(GLOB modulators RELATIVE ${PROJECT_SOURCE_DIR} Modulators/*.txt)
#file(GLOB fields RELATIVE ${PROJECT_SOURCE_DIR} field/*.TABLE)
