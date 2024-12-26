cmake_minimum_required(VERSION 3.10)
project(MyApplication VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/usr/include /opt/homebrew/include /usr/local/include) 
link_directories(/opt/homebrew/lib /opt/homebrew/opt/libpq/lib)

find_package(nlohmann_json 3.10.0 REQUIRED)

file(GLOB SOURCES
    "application/src/*.cpp"
    "application/src/command/*.cpp"
    "application/src/database/*.cpp"
    "application/src/repository/*.cpp"
    "application/src/service/*.cpp"
    "application/src/template/*.cpp"
    "application/src/utils/*.cpp"
    "application/src/umlconverter/*cpp"
)

add_executable(main ${SOURCES})

target_link_libraries(main pqxx pq sqlite3 nlohmann_json::nlohmann_json)

add_custom_target(clean-main COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/main)