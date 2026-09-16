cmake_minimum_required(VERSION 3.16)

# Project name and version
project(MyCppPractice VERSION 1.0)

# Set C++ standard to 20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add platform-specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set toolchain for vcpkg (Windows only)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

    # Find Boost using vcpkg
    find_package(Boost REQUIRED COMPONENTS system)

    # Additional libraries required on Windows
    set(PLATFORM_LIBS ws2_32)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Find Boost using Homebrew (macOS only)
    find_package(Boost REQUIRED)

    # No additional libraries required for macOS
    set(PLATFORM_LIBS "")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Find Boost
    find_package(Boost REQUIRED)

    # Link pthread library for Linux
    set(PLATFORM_LIBS pthread)
endif()

# Include directories
include_directories(${Boost_INCLUDE_DIRS} src)

# Add the executable
# 소스 파일들은 각각 int main(){...} 을 가지고 있기 때문에
# add_executable 안에는 오직 1 개의 소스 파일만 주석 해제 돼 있어야 한다.
add_executable(MyCppPractice
        04-boost_asio/boost_version.cpp
        # 04-boost_asio/completion_token.cpp
)

# Link Boost libraries and platform-specific libraries
target_link_libraries(MyCppPractice ${Boost_LIBRARIES} ${PLATFORM_LIBS})