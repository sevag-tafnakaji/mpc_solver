add_executable(main main.cpp)

target_compile_features(main PUBLIC cxx_std_17)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
