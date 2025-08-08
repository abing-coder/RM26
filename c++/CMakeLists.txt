# CMake最低版本要求
cmake_minimum_required(VERSION 3.20)

# 项目名称
project(main)

# ==================== OpenVINO配置 ====================
# 设置OpenVINO的安装路径
set(OpenVINO_DIR "/home/ubuntu/l_openvino_toolkit_ubuntu22_2024.6.0.17404.4c0f47d2335_x86_64/runtime/cmake")
# 查找并加载OpenVINO包
find_package(OpenVINO REQUIRED)

# ==================== Eigen库配置 ====================

find_package (Eigen3 3.4 REQUIRED NO_MODULE)

# ==================== 包含目录配置 ====================
# OpenCV头文件包含目录
find_package(OpenCV REQUIRED)

# 设置OpenCV的头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ncnn/cpp/include)


# ==================== 模块化源文件收集 ====================
# 收集所有模块源文件
set(MODULE_SOURCES
    src/Detector.cpp
    src/ImageProcessor.cpp
    src/Visualizer.cpp
    src/PerformanceMonitor.cpp
    src/Config.cpp
)

# ==================== BYTETracker源文件收集 ====================
# 收集BYTETracker相关源文件
set(BYTETRACKER_SOURCES
    ncnn/cpp/src/BYTETracker.cpp
    ncnn/cpp/src/STrack.cpp
    ncnn/cpp/src/kalmanFilter.cpp
    ncnn/cpp/src/lapjv.cpp
    ncnn/cpp/src/utils.cpp
)

# ==================== 可执行文件配置 ====================

# 模块化版本（包含BYTETracker）
add_executable(modular_main src/main_modular.cpp ${MODULE_SOURCES} ${BYTETRACKER_SOURCES})
target_link_libraries(modular_main  openvino::runtime)
target_link_libraries(modular_main ${OpenCV_LIBS})
target_link_libraries (modular_main Eigen3::Eigen)
