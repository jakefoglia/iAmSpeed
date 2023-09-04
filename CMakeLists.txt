# Linux example...
# cmake ..
# make -j

# Windows MinGW example...
# cmake -G 'Visual Studio 17 2022' ..
# MSBuild.exe ./iAmSpeed.sln
# OR
# cmake --build .


cmake_minimum_required(VERSION 3.22.0)
project(iAmSpeed C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(Vulkan REQUIRED)
message("[iAmSpeed]: Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
message("[iAmSpeed]: Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")

# DSAA
add_subdirectory(vendor/DSAA)

# GLFW
add_subdirectory(vendor/glfw)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

# Dear imgui
add_library(imgui
  vendor/imgui/imgui_demo.cpp
  vendor/imgui/imgui_draw.cpp
  vendor/imgui/imgui_tables.cpp
  vendor/imgui/imgui_widgets.cpp
  vendor/imgui/imgui.cpp
  vendor/imgui/backends/imgui_impl_vulkan.cpp
  vendor/imgui/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui
  PUBLIC vendor/imgui
  PRIVATE vendor/glfw/deps
  PUBLIC vendor/imgui/backends
  INTERFACE vendor/glfw/include
  PUBLIC ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(imgui
  PUBLIC glfw # public so out top level targets just need to link to  imgui and they automatically get the glfw / vulkan stuff 
  PUBLIC ${Vulkan_LIBRARIES}
)

add_library(Interface
  src/Interface/Interface.cpp
)
target_include_directories(Interface
  PUBLIC include/Interface
  PUBLIC vendor/DSAA/include
)
target_link_libraries(Interface
  PUBLIC imgui
  PUBLIC DSAA
)

# set_target_properties(${PROJECT_NAME} PROPERTIES
#   LINK_SEARCH_START_STATIC ON
#   LINK_SEARCH_END_STATIC ON
# )

target_link_libraries(${PROJECT_NAME} 
  PRIVATE Interface
)