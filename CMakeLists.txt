# Minimum CMake version required
cmake_minimum_required(VERSION 3.12)

# Set the project name and version
project(Zing VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directory to the search path for header files
include_directories(${PROJECT_SOURCE_DIR}/App/include)

# Collect all .cpp files in the src folder (including subdirectories)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/App/src/*.cpp)

# Create the executable including main.cpp from the project root
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/App/main.cpp ${SOURCES})

# Find Boost with the system component (required by Boost.Asio)
find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    # Add Boost include directories
    include_directories(${Boost_INCLUDE_DIRS})
    
    # Link Boost.System to your target
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
endif()