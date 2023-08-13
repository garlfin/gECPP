cmake_minimum_required(VERSION 3.25)
project(VoxelField)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -O0)

include_directories(gECPP Vendor Vendor/FBX)

# GLFW
add_library(GLFW STATIC IMPORTED)
set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll")
set_property(TARGET GLFW PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.lib")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ASSIMP
add_library(ASSIMP STATIC IMPORTED)
set_property(TARGET ASSIMP PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# GLAD
add_library(GLAD STATIC Vendor/GLAD/glad.c Vendor/GLAD/glad.h)

# GEMODEL
file(GLOB gEMODEL_SRC gEModel/Converter)
add_executable(gEModel gEModel/main.cpp ${gEMODEL_SRC} gEModel/gETF.h)
target_link_libraries(gEModel PRIVATE ASSIMP)

# GECPP
file(GLOB GECPP_SRC gECPP/*.cpp gECPP/*.h)
add_library(gECPP STATIC ${GECPP_SRC})
target_link_libraries(gECPP GLAD GLFW)

# VOXELDEMO
add_executable(VoxelDemo Demo/main.cpp Demo/DemoWindow.h Demo/DemoWindow.cpp)
target_link_libraries(VoxelDemo gECPP)
