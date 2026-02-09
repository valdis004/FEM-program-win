add_executable(FEM-program
    main_window.cpp
    main.cpp
    context_menu/tree_context_menu.cpp
     ../graphics/qtgl/qtgl.cpp
     # модули FEM
     ${CMAKE_HOME_DIRECTORY}/src/mesh/mesh.cpp
     ${CMAKE_HOME_DIRECTORY}/src/mesh/meshdata.cpp
     ${CMAKE_HOME_DIRECTORY}/src/general_element/plates/plate.cpp
     ${CMAKE_HOME_DIRECTORY}/src/general_element/load/load.cpp
     ${CMAKE_HOME_DIRECTORY}/src/general_element/displacement/displacement.cpp
     ${CMAKE_HOME_DIRECTORY}/src/general_element/element.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/fem_plates/fem_plate_mitc4my.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/fem_plates/fem_plate_mitc9my.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/fem_plates/fem_plate_dkmq.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/fem_element.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/load/fem_load.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/displacement/fem_displacement.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/element_provider.cpp
     ${CMAKE_HOME_DIRECTORY}/src/solver/solver.cpp
     ${CMAKE_HOME_DIRECTORY}/src/fem_elements/material/plate_Material.cpp
)


target_include_directories(FEM-program
    PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/app
        ${PROJECT_SOURCE_DIR}/src/context_menu
        ${PROJECT_SOURCE_DIR}/src/fem_elements
        ${PROJECT_SOURCE_DIR}/src/fem_elements/load
        ${PROJECT_SOURCE_DIR}/src/fem_elements/displacement
        ${PROJECT_SOURCE_DIR}/src/fem_elements/material
        ${PROJECT_SOURCE_DIR}/src/fem_elements/fem_plates
        ${PROJECT_SOURCE_DIR}/src/graphics
        ${PROJECT_SOURCE_DIR}/src/mesh
        ${PROJECT_SOURCE_DIR}/src/solver

        ${PROJECT_SOURCE_DIR}/src/general_element
        ${PROJECT_SOURCE_DIR}/src/general_element/load
        ${PROJECT_SOURCE_DIR}/src/general_element/displacement
)


target_link_libraries(FEM-program PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Gui Eigen3::Eigen opengl32)

set_target_properties(FEM-program PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)