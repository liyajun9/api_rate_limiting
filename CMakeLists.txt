cmake_minimum_required(VERSION 3.15)
add_subdirectory(server_side/method_or_ip)

add_executable(${PROJECT_NAME} )

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g -O3 -std=c++0x)
target_compile_definitions(${PROJECT_NAME} PRIVATE '_LINUX_OS_', 'RAPIDJSON_HAS_STDSTRING', 'USE_GMLOGAPI')

#target_include_directories(${PROJECT_NAME} PRIVATE )
#
#target_link_directories(${PROJECT_NAME} PRIVATE )
#
#target_link_libraries(${PROJECT_NAME} PRIVATE )