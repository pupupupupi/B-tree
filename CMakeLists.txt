# 设置最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(MyProject VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 指定源文件所在的目录
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src) # 假设你的源文件在src目录中

# 查找所有的源文件
file(GLOB SOURCES ${SOURCE_DIR}/*.cc)

# 检查是否找到源文件
if(SOURCES)
    # 创建一个可执行文件，并指定源文件
    add_executable(MyExecutable ${SOURCES})
else()
    message(FATAL_ERROR "No C++ source files found in ${SOURCE_DIR}")
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 如果你需要额外的编译选项，可以在这里添加
# target_compile_options(MyExecutable PRIVATE -Wall -O2)

# 如果你的项目依赖其他库，可以在这里链接
# target_link_libraries(MyExecutable someLibrary)
