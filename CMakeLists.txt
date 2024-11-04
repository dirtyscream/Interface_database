cmake_minimum_required(VERSION 3.10)
project(MyApplication VERSION 1.0 LANGUAGES CXX)

# Установка стандарта C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указание каталогов для включаемых файлов и библиотек
include_directories(/usr/include /opt/homebrew/include)
link_directories(/opt/homebrew/lib /opt/homebrew/opt/libpq/lib)

# Поиск всех исходных файлов в указанных директориях
file(GLOB SOURCES
    "application/src/*.cpp"
    "application/src/command/*.cpp"
    "application/src/database/*.cpp"
    "application/src/repository/*.cpp"
    "application/src/service/*.cpp"
    "application/src/template/*.cpp"
    "application/src/utils/*.cpp"
)

# Создание исполняемого файла и связывание с библиотеками
add_executable(main ${SOURCES})
target_link_libraries(main pqxx pq sqlite3)

# Кастомная цель для очистки сборки
add_custom_target(clean-main COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/main)
