cmake_minimum_required(VERSION 3.16)
project("TestGame")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Binaries)

file(GLOB_RECURSE SRC_FILES "Source/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

add_subdirectory("Engine")
target_link_libraries(${PROJECT_NAME} PRIVATE Otherengine-Engine)

install(DIRECTORY "Binaries" DESTINATION ".")
install(DIRECTORY "Assets" DESTINATION ".")
