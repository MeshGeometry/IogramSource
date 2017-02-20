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

#make sure append this to debug
set(CMAKE_DEBUG_POSTFIX "_d")

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (Urho3D-CMake-common)

# Find Urho3D library
find_package (Urho3D REQUIRED)
include_directories (${URHO3D_INCLUDE_DIRS})

#include third party stuff
include_directories("../ThirdParty")
include_directories("../ThirdParty/Libigl")
include_directories("../ThirdParty/Eigen")
include_directories("../ThirdParty/TetGen")

# Define target name
set (TARGET_NAME Geometry)

#define includes
include_directories("./")
include_directories("../ThirdParty/Assimp/include")

# Define source files
define_source_files ()

list(APPEND SOURCE_FILES 
    "../ThirdParty/TetGen/tetgen.cxx"
    "../ThirdParty/TetGen/predicates.cxx")

file(GLOB SHAPEOP_SRC
    "../ThirdParty/ShapeOp/ShapeOp_*")

file(GLOB SHERKCOLLINS_SRC
    "../ThirdParty/SherkCollins/*.*")

list(APPEND SOURCE_FILES ${SHAPEOP_SRC} ${SHERKCOLLINS_SRC})

#get rid of resource copying
set(RESOURCE_DIRS "")

# Setup target with resource copying
setup_library ()

set_target_properties(Geometry PROPERTIES LINKER_LANGUAGE CXX)

#### RELEAE COPYING ######
install(TARGETS Geometry DESTINATION ${CMAKE_SOURCE_DIR}/SDK/lib )
