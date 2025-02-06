cmake_minimum_required(VERSION 3.12)

project(ZingClient VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp ${SOURCES})

find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAPNP REQUIRED capnp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CAPNP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CAPNP_LIBRARIES})
