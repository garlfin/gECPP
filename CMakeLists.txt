cmake_minimum_required(VERSION 3.20)
project(VoxelField)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -static -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -static -s")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -static")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -static")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(gECPP Vendor Vendor/FBX ${CMAKE_SOURCE_DIR})

# GLFW
add_library(GLFW STATIC IMPORTED)
set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll")
set_property(TARGET GLFW PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.lib")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ASSIMP
add_library(ASSIMP STATIC IMPORTED gECPP/GL/Binary.cpp Demo/Engine/Renderer/VoxelPipeline.h)
set_property(TARGET ASSIMP PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# GLAD
add_library(GLAD STATIC Vendor/GLAD/glad.c Vendor/GLAD/glad.h)

# GETF
add_library(GETF STATIC gEModel/gETF.h gEModel/gETF/gETF.cpp gECPP/GL/Binary.h gEModel/gETF/File.h gEModel/gETF/Prototype.h gECPP/GL/Binary.cpp)

# GEMODEL
file(GLOB_RECURSE gEMODEL_SRC gEModel/Converter)
add_executable(gEModel gEModel/main.cpp ${gEMODEL_SRC})
target_link_libraries(gEModel PRIVATE ASSIMP GETF)

# GECPP
file(GLOB_RECURSE GECPP_SRC "gECPP/*.cpp" "gECPP/*.h")
add_library(gECPP STATIC ${GECPP_SRC} )
target_link_libraries(gECPP GLAD GLFW GETF)

# VOXELDEMO
file(GLOB_RECURSE VOXELDEMO_SRC "Demo/*.cpp" "Demo/*.h")
add_executable(VoxelDemo ${VOXELDEMO_SRC})
target_link_libraries(VoxelDemo gECPP)
add_custom_command(TARGET VoxelDemo PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Resource/ ${CMAKE_BINARY_DIR}/Resource/)
