cmake_minimum_required(VERSION 3.15)
project(method_or_ip)

add_executable(${PROJECT_NAME} LruSharedCache.cpp StringUtility.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g -O2 -std=c++0x)
target_compile_definitions(${PROJECT_NAME} PRIVATE '_LINUX_OS_', 'RAPIDJSON_HAS_STDSTRING', 'USE_GMLOGAPI')

target_include_directories(${PROJECT_NAME} PRIVATE
        ./
        /usr/include/
        /usr/local/include/)

target_link_directories(${PROJECT_NAME} PRIVATE /usr/lib
        /usr/local/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE -pthread)