qt_add_executable(FEM-program
    mainwindow.cpp
    main.cpp
)

target_link_libraries(FEM-program PRIVATE Qt6::Widgets)

set_target_properties(FEM-program PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

#add_executable(FEM-program src/main/main.cpp)