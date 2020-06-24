cmake_minimum_required(VERSION 3.16)
project("TestGame")

add_subdirectory("Engine")

file(GLOB_RECURSE SRC_FILES "Source/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Otherengine-Engine)

# Temporary
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

install(DIRECTORY "Binaries" DESTINATION ".")
install(DIRECTORY "Assets" DESTINATION ".")
