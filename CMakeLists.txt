cmake_minimum_required(VERSION 3.20)
project(VoxelField)

set(CMAKE_CXX_STANDARD 23)

set(COMPILE_FLAGS_RELEASE "/O2")
set(COMPILE_FLAGS_DEBUG "/Ob0 /DDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILE_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILE_FLAGS_RELEASE}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILE_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMPILE_FLAGS_DEBUG}")

include_directories(gECPP Vendor Vendor/FBX ${CMAKE_SOURCE_DIR})

# JOLT
file(GLOB_RECURSE JOLT_SRC "Vendor/JOLT/*.cpp" "Vendor/JOLT/*.h")
add_library(JOLT STATIC ${JOLT_SRC})

# GLFW
add_library(GLFW STATIC IMPORTED)
set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ASSIMP
add_library(ASSIMP STATIC IMPORTED)
set_property(TARGET ASSIMP PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/assimp-vc143-mt.lib")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/assimp-vc143-mt.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# GLAD
add_library(GLAD STATIC Vendor/GLAD/glad.c Vendor/GLAD/glad.h)

# PUGIXML
add_library(PUGIXML STATIC Vendor/PUGIXML/pugixml.cpp Vendor/PUGIXML/pugixml.hpp Vendor/PUGIXML/pugiconfig.hpp)

# GEUI
add_executable(gEUI gEUI/main.cpp)
target_link_libraries(gEUI PUGIXML GLAD)

# GECPP
file(GLOB_RECURSE GECPP_SRC "gECPP/*.cpp" "gECPP/*.h" "gECPP/*.inl")

add_library(gECPP STATIC ${GECPP_SRC})
target_compile_definitions(gECPP PUBLIC API_ID=API_GL)
target_link_libraries(gECPP GLAD GLFW JOLT)

# VOXELDEMO
file(GLOB_RECURSE VOXELDEMO_SRC "Demo/*.cpp" "Demo/*.h" "Demo/*.inl")
add_executable(VoxelDemo ${VOXELDEMO_SRC})
target_link_libraries(VoxelDemo gECPP)

# GEMODEL
file(GLOB_RECURSE GEMODEL_SRC "gEModel/*.cpp" "gEModel/*.h" "gEModel/*.inl")
add_executable(gEModel ${GEMODEL_SRC})
target_link_libraries(gEModel GLAD gECPP ASSIMP)