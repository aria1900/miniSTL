cmake_minimum_required(VERSION 3.0)
# 包含的头文件
include_directories(${PROJECT_SOURCE_DIR}/MySTL)
# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 执行输出可执行文件的名字
add_executable(stltest ./Test/test.cpp)