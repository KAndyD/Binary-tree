# Указываем минимальную требуемую версию CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(lab4)

# Указываем стандарт C++ (например, C++17)
set(CMAKE_CXX_STANDARD 17)

# Добавляем исполняемый файл с исходными файлами
add_executable(lab4 main.cpp binary_tree.cpp)

# Указываем дополнительные директории с заголовочными файлами (если они не в том же каталоге)
target_include_directories(lab4 PRIVATE ${CMAKE_SOURCE_DIR})

# Если у вас есть дополнительные библиотеки, можете их подключить здесь, например:
# target_link_libraries(lab4 some_library)
