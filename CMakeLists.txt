cmake_minimum_required(VERSION 3.20)
project(VoxelField)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -static -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -static -s")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -static -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -static")

include_directories(gECPP Vendor Vendor/FBX ${CMAKE_SOURCE_DIR})

# GLFW
add_library(GLFW STATIC IMPORTED)
set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll")
set_property(TARGET GLFW PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.lib")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/glfw3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ASSIMP
add_library(ASSIMP STATIC IMPORTED gECPP/gETF/GUI/Stylesheet.h gEUI/main.cpp)
set_property(TARGET ASSIMP PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ASSIMP/libassimp-5.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# GLAD
add_library(GLAD STATIC Vendor/GLAD/glad.c Vendor/GLAD/glad.h)

# GETF
add_library(GETF STATIC gECPP/gETF/File.h gECPP/gETF/File.cpp gECPP/gETF/Mesh/Mesh.h gECPP/gETF/Mesh/Mesh.cpp gECPP/GL/Binary/Binary.h gECPP/gETF/File.h gECPP/gETF/Serializable.h gECPP/GL/Binary/Binary.cpp gECPP/GL/Buffer/VAO.cpp gECPP/gETF/GUI/Stylesheet.h)

# PUGIXML
add_library(PUGIXML STATIC Vendor/PUGIXML/pugixml.cpp Vendor/PUGIXML/pugixml.hpp Vendor/PUGIXML/pugiconfig.hpp)

# GEMODEL
add_executable(gEModel gEModel/main.cpp gEModel/Vertex.h)
target_link_libraries(gEModel PRIVATE ASSIMP GETF GLAD)
# gotta link glad or it yaps about linking

# GEUI
add_executable(gEUI gEUI/main.cpp)
target_link_libraries(gEUI PRIVATE PUGIXML GETF GLAD)

# GECPP
file(GLOB_RECURSE GECPP_SRC "gECPP/*.cpp" "gECPP/*.h")
add_library(gECPP STATIC ${GECPP_SRC} gECPP/GL/Type.h)
target_link_libraries(gECPP GLAD GLFW GETF)

# VOXELDEMO
file(GLOB_RECURSE VOXELDEMO_SRC "Demo/*.cpp" "Demo/*.h")
add_executable(VoxelDemo ${VOXELDEMO_SRC})
target_link_libraries(VoxelDemo gECPP)