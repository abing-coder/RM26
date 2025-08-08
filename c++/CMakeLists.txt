# CMake最低版本要求
cmake_minimum_required(VERSION 3.20)

# 项目名称
project(DetectionSystem)

# ==================== OpenVINO配置 ====================
# 设置OpenVINO的安装路径
set(OpenVINO_DIR "C:/w_openvino_toolkit_windows_2024.6.0.17404.4c0f47d2335_x86_64/runtime/cmake")
# 查找并加载OpenVINO包
find_package(OpenVINO REQUIRED)

# ==================== Eigen库配置 ====================
# 直接包含Eigen库头文件目录
include_directories("D:/detect26_C++/eigen-3.4.0")

# ==================== 包含目录配置 ====================
# OpenCV头文件包含目录
include_directories("C:/opencv/build/include")
include_directories("C:/opencv/build/include/opencv2")
# 项目自身的头文件包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ncnn/cpp/include)

# ==================== 库目录配置 ====================
# OpenCV库文件目录（64位Visual Studio 2019/2022版本）
link_directories("C:/opencv/build/x64/vc16/lib")

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
# 原始版本（单文件）
# add_executable(original_main src/main.cpp)
# target_link_libraries(original_main opencv_world4110d.lib openvino::runtime)

# 模块化版本（包含BYTETracker）
add_executable(modular_main src/main_modular.cpp ${MODULE_SOURCES} ${BYTETRACKER_SOURCES})
target_link_libraries(modular_main opencv_world4110d.lib openvino::runtime)



#   msbuild DetectionSystem.sln /p:Configuration=Debug(vs2022集成终端可用此命令)
# 
# 可执行文件：
#   - original_main.exe: 原始单文件版本
#   - modular_main.exe: 模块化重构版本（包含BYTETracker跟踪）