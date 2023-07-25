# Example usage:
#  mkdir build
#  cd build5
#  cmake -g "Visual Studio 14 2015" ..

# cmake -G 'Visual Studio 17 2022' ../src
# MSBuild.exe ./sorts_benchmarker.sln
# OR
# cmake --build .


cmake_minimum_required(VERSION 3.22.0)

project(sorts_benchmarker C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# GLFW
set(GLFW_DIR ./glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR ./imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# Libraries
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
#find_library(VULKAN_LIBRARY
  #NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
set(LIBRARIES "glfw;Vulkan::Vulkan")


# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)


#file(GLOB sources *.cpp)
#add_executable(${PROJECT_NAME} ${sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)



add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(imgui)
add_subdirectory(Application)


target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${LIBRARIES} Application imgui)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DImTextureID=ImU64)



# My stuff
