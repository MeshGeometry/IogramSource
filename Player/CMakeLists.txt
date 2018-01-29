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
    # MACOSX_BUNDLE path is set by default 
    cmake_policy (SET CMP0006 OLD)
endif ()

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (UrhoCommon)

# Define target name
set (TARGET_NAME Player)

# Define source files
define_source_files()

#get rid of resource copying
set(RESOURCE_DIRS "")

# Setup target with resource copying
if (APPLE)
setup_main_executable (MACOSX_BUNDLE)
  else()
setup_main_executable ()
endif()

add_definitions(-DNOMINMAX)
#add_definitions(-DSHAPEOP_HEADER_ONLY)

#link the libs
include_directories("../Core")
include_directories("../Geometry")
include_directories("../Components")

#mandatory libs
target_link_libraries(Player Components)
target_link_libraries(Player Core)
target_link_libraries(Player Geometry)

if (APPLE)
target_link_libraries(${TARGET_NAME} "-framework SystemConfiguration")
endif()


if(WIN32)
    install(FILES ${CMAKE_BINARY_DIR}/bin/Player.exe DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/win)
elseif(EMSCRIPTEN)
    install(FILES ${CMAKE_BINARY_DIR}/bin/Player.js  ${CMAKE_BINARY_DIR}/bin/Player.html.mem DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/webgl)
elseif(APPLE)
    install(FILES ${CMAKE_BINARY_DIR}/bin/Player DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/osx)
elseif(UNIX)
    install(FILES ${CMAKE_BINARY_DIR}/bin/Player DESTINATION ${CMAKE_SOURCE_DIR}/bin/Runtime/linux)
endif()
