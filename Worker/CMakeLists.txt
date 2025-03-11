cmake_minimum_required(VERSION 3.12)

project(ZingWorker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# get the request.capnp.cpp file from App
set(APP_SOURCES ../App/src/request.capnp.c++)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp ${SOURCES} ${APP_SOURCES})

# copu request.capnp.h to build directory
configure_file(../App/include/request.capnp.h ${CMAKE_BINARY_DIR}/request.capnp.h COPYONLY)
# tell compiler to look for headers in the build directory
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}) 

find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
endif()

find_package(PkgConfig REQUIRED)

# Include capnp
pkg_check_modules(CAPNP REQUIRED capnp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CAPNP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CAPNP_LIBRARIES})

# Include rdkafka (ensure both C++ and C versions are linked)
pkg_check_modules(RDKAFKA REQUIRED rdkafka++ rdkafka)
target_include_directories(${PROJECT_NAME} PRIVATE ${RDKAFKA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RDKAFKA_LIBRARIES} rdkafka rdkafka++)