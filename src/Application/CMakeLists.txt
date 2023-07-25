add_library(Application Application.cpp)

target_include_directories(Application
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"                          
)

add_subdirectory(Sorts)

target_link_libraries(Application PUBLIC Sorts)