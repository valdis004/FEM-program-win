cmake_minimum_required(VERSION 3.10.0)
project(FEM-program VERSION 0.1.0 LANGUAGES C CXX)

set(Qt6_DIR "C:/msys64/mingw64/lib/cmake/Qt6")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_subdirectory(src/app)

