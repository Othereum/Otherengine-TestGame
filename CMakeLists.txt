cmake_minimum_required(VERSION 3.8)
project("TestGame")

file(GLOB_RECURSE SRC_FILES "Source/Private/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_subdirectory("Engine")

target_include_directories(${PROJECT_NAME} PUBLIC "Source/Public")
target_link_libraries(${PROJECT_NAME} PRIVATE Otherengine)
