# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required (VERSION 3.2.3)
if (COMMAND cmake_policy)
    # Libraries linked via full path no longer produce linker search paths
    cmake_policy (SET CMP0003 NEW)
    # INTERFACE_LINK_LIBRARIES defines the link interface
    cmake_policy (SET CMP0022 NEW)
    # Disallow use of the LOCATION target property - so we set to OLD as we still need it
    cmake_policy (SET CMP0026 OLD)
    # MACOSX_RPATH is enabled by default
    cmake_policy (SET CMP0042 NEW)
    # Honor the visibility properties for SHARED target types only
    cmake_policy (SET CMP0063 OLD)
endif ()

#make sure append this to debug
set(CMAKE_DEBUG_POSTFIX "_d")

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (UrhoCommon)

# Define target name
set (TARGET_NAME Components)

#c11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-DNOMINMAX)
#add_definitions(-DSHAPEOP_HEADER_ONLY)

#define includes
include_directories("../ThirdParty")
include_directories("../ThirdParty/Libigl")
include_directories("../ThirdParty/Eigen")
include_directories("../ThirdParty/ShapeOp")
include_directories("../ThirdParty/Assimp/include")
include_directories("../UI")
include_directories("../Core")
include_directories("../Geometry")

# Define source files
#define_source_files (RECURSE GROUP)
define_source_files ()

#create some groups for sanity
file(GLOB Mesh_SRC "./Mesh_*")
source_group("Mesh" FILES ${Mesh_SRC})
file(GLOB Math_SRC "./Maths_*")
source_group("Maths" FILES ${Math_SRC})
file(GLOB Input_SRC "./Input_*")
source_group("Input" FILES ${Input_SRC})
file(GLOB Graphics_SRC "./Graphics_*")
source_group("Graphics" FILES ${Graphics_SRC})
file(GLOB Curve_SRC "./Curve_*")
source_group("Curve" FILES ${Curve_SRC})
file(GLOB Geometry_SRC "./Geometry_*")
source_group("Geometry" FILES ${Geometry_SRC})
file(GLOB Interop_SRC "./Interop_*")
source_group("Interop" FILES ${Interop_SRC})
file(GLOB Scene_SRC "./Scene_*")
source_group("Scene" FILES ${Scene_SRC})
file(GLOB Sets_SRC "./Sets_*")
source_group("Sets" FILES ${Sets_SRC})
file(GLOB ShapeOp_SRC "./ShapeOp_*")
source_group("ShapeOp" FILES ${ShapeOp_SRC})
file(GLOB Spatial_SRC "./Spatial_*")
source_group("Spatial" FILES ${Spatial_SRC})
file(GLOB Tree_SRC "./Tree_*")
source_group("Tree" FILES ${Tree_SRC})
file(GLOB Vector_SRC "./Vector_*")
source_group("Vector" FILES ${Vector_SRC})
file(GLOB Widget_SRC "./Widget_*")
source_group("Widget" FILES ${Widget_SRC})
file(GLOB Physics_SRC "./Physics_*")
source_group("Physics" FILES ${Physics_SRC})
file(GLOB Misc_SRC 
    "./ColorDefs.*"
    "./RegisterCoreComponents.*"
    "./OrbitCamera.*"
    "./ReflectionProbe.*"
    "./ColorSlider.*"
    "./MultiSlider.*"
    "./TransformEdit.*"
    "./ModelEdit.*"
    "./ModelEditLinear.*"
    "./ModelBoxMorph.*"
    "./MeshSketcher.*"
    )
source_group("Misc" FILES ${Misc_SRC})

#get rid of resource copying
set(RESOURCE_DIRS "")

# Setup target with resource copying
setup_library ()

target_link_libraries(Components Core)
target_link_libraries(Components Geometry)

#compile flags
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()
