# CMakeList.txt: проект CMake для CMakeTask; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("CMakeTask")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (CMakeTask "CMakeTask.cpp" "CMakeTask.h" "Context.h" "Context.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
