cmake_minimum_required(VERSION 3.15)
project(mpc_solver VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include_directories(osqp-cpp/include)

add_subdirectory(src)
