# add_executable(main main.cpp)

# target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
# target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

add_executable(${PROJECT_NAME} 
    main.cpp 
    Plotting/resource_manager.cpp
    Plotting/shader.cpp
    Plotting/plotter.cpp
    Plotting/utils.cpp
    Plotting/glad.c)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wfloat-conversion)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

target_link_libraries(${PROJECT_NAME}
    # here you can add any library dependencies
    glfw dl GL freetype
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME}_destination)
