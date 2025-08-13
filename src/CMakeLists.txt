
add_executable(${PROJECT_NAME} 
    main.cpp
)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wfloat-conversion)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

target_link_libraries(${PROJECT_NAME}
    # here you can add any library dependencies
    Plotter glfw dl GL freetype
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME}_destination)
