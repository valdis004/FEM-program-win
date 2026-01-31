add_executable(FEM-program
    mainwindow.cpp
    main.cpp
    context-menu/treeContextMenu.cpp
     ../graphics/qtgl/qtgl.cpp
     # модули FEM
     ${CMAKE_HOME_DIRECTORY}/src/mesh/mesh.cpp
     ${CMAKE_HOME_DIRECTORY}/src/mesh/meshdata.cpp
     ${CMAKE_HOME_DIRECTORY}/src/generalElement/plates/plate.cpp
     ${CMAKE_HOME_DIRECTORY}/src/generalElement/load/load.cpp
     ${CMAKE_HOME_DIRECTORY}/src/generalElement/displacement/displacement.cpp
     ${CMAKE_HOME_DIRECTORY}/src/generalElement/element.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/plates/plates.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/femelement.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/load/femload.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/displacement/femdisplacement.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/elementprovider.cpp
     ${CMAKE_HOME_DIRECTORY}/src/solver/solver.cpp
     ${CMAKE_HOME_DIRECTORY}/src/elements/material/PlateMaterial.cpp
)


target_include_directories(FEM-program
    PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/app
        ${PROJECT_SOURCE_DIR}/src/app/context-menu
        ${PROJECT_SOURCE_DIR}/src/elements
        ${PROJECT_SOURCE_DIR}/src/elements/load
        ${PROJECT_SOURCE_DIR}/src/elements/displacement
        ${PROJECT_SOURCE_DIR}/src/elements/material
        ${PROJECT_SOURCE_DIR}/src/elements/plates
        ${PROJECT_SOURCE_DIR}/src/graphics
        ${PROJECT_SOURCE_DIR}/src/mesh
        ${PROJECT_SOURCE_DIR}/src/solver

        ${PROJECT_SOURCE_DIR}/src/generalElement
        ${PROJECT_SOURCE_DIR}/src/generalElement/load
        ${PROJECT_SOURCE_DIR}/src/generalElement/displacement
)


target_link_libraries(FEM-program PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Gui Eigen3::Eigen opengl32)

set_target_properties(FEM-program PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)