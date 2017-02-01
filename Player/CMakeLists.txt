# Set minimum version
cmake_minimum_required (VERSION 2.8.6)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0006 OLD)
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

# Define target name
set (TARGET_NAME IogramPlayer)
# Define source files
#SET(IOLIB "../IogramLib")
#define_source_files (GLOB_CPP_PATTERNS ./*.cpp ${IOLIB}/Graph/*.cpp ${IOLIB}/Components/*.cpp ${IOLIB}/Geometry/*.cpp ${IOLIB}/Render/*.cpp ${IOLIB}/Editor/*.cpp)
define_source_files()

#get rid of resource copying
set(RESOURCE_DIRS "")

# Setup target with resource copying
setup_main_executable ()

add_definitions(-DNOMINMAX)
#add_definitions(-DSHAPEOP_HEADER_ONLY)

#link the libs
include_directories("../Core")
include_directories("../Geometry")
include_directories("../Components")
#include_directories("../ThirdParty/ShapeOp/src")

target_link_libraries(IogramPlayer Components)
target_link_libraries(IogramPlayer Core)
target_link_libraries(IogramPlayer Geometry)
#target_link_libraries(IogramPlayer ShapeOp)
target_link_libraries(IogramPlayer Assimp)


# #compile flags
# if(UNIX)
#     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
# endif()


# if(WIN32)
#     install(TARGETS IogramPlayer DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/win)
# elseif(EMSCRIPTEN)
#     install(TARGETS IogramPlayer DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/webgl)
# elseif(APPLE)
#     install(TARGETS IogramPlayer DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/osx)
# elseif(UNIX)
#     install(TARGETS IogramPlayer DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/linux)
# endif()
