cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # works


project(FEM-program VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Gui)
find_package(Eigen3 REQUIRED)

qt_standard_project_setup()

add_subdirectory(src/app)

