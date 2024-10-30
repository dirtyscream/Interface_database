cmake_minimum_required(VERSION 3.10)

# Укажите имя вашего проекта
project(MyApplication VERSION 1.0 LANGUAGES CXX)

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# Укажите директории для поиска заголовочных файлов
include_directories(/usr/include /opt/homebrew/include)

# Укажите директории для поиска библиотек
link_directories(/opt/homebrew/lib /opt/homebrew/opt/libpq/lib)

# Укажите исходные файлы
file(GLOB SOURCES
    "application/src/*.cpp"
    "application/src/command/*.cpp"
    "application/src/database/*.cpp"
    "application/src/repository/*.cpp"
    "application/src/service/*.cpp"
    "application/src/template/*.cpp"
    "application/src/utils/*.cpp")

# Создайте исполняемый файл
add_executable(main ${SOURCES})

# Свяжите библиотеки
target_link_libraries(main pqxx pq sqlite3)

# Установите правила для очистки
set(CMAKE_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/main")
