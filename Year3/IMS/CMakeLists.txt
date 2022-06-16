cmake_minimum_required(VERSION 3.16)
project(GreenDeal)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug)

add_subdirectory(src/ArgParser)
add_subdirectory(src/GreenDealSimulation)
add_executable(GreenDeal ims.cpp)

target_link_libraries(GreenDeal PUBLIC ArgParser)
target_link_libraries(GreenDeal PUBLIC GreenDealSimulation)

target_include_directories(GreenDeal PUBLIC
        src/ArgParser
        src/GreenDealSimulation)
