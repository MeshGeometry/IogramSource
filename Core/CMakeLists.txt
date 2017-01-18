# Set minimum version
cmake_minimum_required (VERSION 2.8.6)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (Urho3D-CMake-common)

# Find Urho3D library
find_package (Urho3D REQUIRED)
include_directories (${URHO3D_INCLUDE_DIRS})

#include third party stuff


# Define target name
set (TARGET_NAME Core)

#c11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-DNOMINMAX)

#define includes
include_directories("./")
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Libigl)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Eigen)
#include_directories("../ThirdParty/ShapeOp/api")
#include_directories("../ThirdParty/Assimp/include")

# Define source files
#define_source_files (RECURSE GROUP)
#get rid of resource copying
set(RESOURCE_DIRS "")
define_source_files ()

# Setup target with resource copying
setup_library ()

#compile flags
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()