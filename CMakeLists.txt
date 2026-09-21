cmake_minimum_required(VERSION 3.16)

# Project name and version
project(MyCppPractice VERSION 1.0)

# Set C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
        # 03-concurrency/01_fill_array.cpp
        # 03-concurrency/02_fill_map.cpp
        # 03-concurrency/03_fill_map_no_mutex.cpp
        # 03-concurrency/04_cache_integer_with_future.cpp
        # 03-concurrency/05_exception_in_async.cpp
        # 03-concurrency/06_blocking_queue_mutex_only.cpp
        # 03-concurrency/07_blocking_queue_mutex_cv.cpp

        # 04-boost_asio/boost_version.cpp
        # 04-boost_asio/completion_token.cpp

        # 05-coroutine/01_coroutine_version_test.cpp
        # 05-coroutine/02_serial_generator.cpp
        # 05-coroutine/03_fibonacci_generator.cpp
        05-coroutine/04_cpp20_serial.cpp
)

# Link Boost libraries and platform-specific libraries
target_link_libraries(MyCppPractice ${Boost_LIBRARIES} ${PLATFORM_LIBS})