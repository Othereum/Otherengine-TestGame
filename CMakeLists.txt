cmake_minimum_required(VERSION 3.14)
project("TestGame")

add_subdirectory("Engine")

file(GLOB_RECURSE SRC_FILES "Source/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Otherengine-Engine)
install(TARGETS ${PROJECT_NAME} DESTINATION "Binaries")
oe_target_setup(${PROJECT_NAME})
