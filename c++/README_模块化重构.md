# 检测系统模块化重构

## 概述

本项目将原始的单一文件检测系统重构为模块化架构，提高了代码的可维护性、可扩展性和可重用性。

## 模块结构

### 1. 配置模块 (Config)
- **文件**: `include/Config.h`, `src/Config.cpp`
- **功能**: 管理所有配置参数
- **主要特性**:
  - 模型路径、视频路径、设备类型等参数管理
  - 支持从文件加载和保存配置
  - 提供参数验证和默认值

### 2. 检测器模块 (Detector)
- **文件**: `include/Detector.h`, `src/Detector.cpp`
- **功能**: 负责模型推理和检测逻辑
- **主要特性**:
  - OpenVINO模型初始化和推理
  - 图像预处理和后处理
  - 检测结果解析和过滤

### 3. 图像处理模块 (ImageProcessor)
- **文件**: `include/ImageProcessor.h`, `src/ImageProcessor.cpp`
- **功能**: 处理图像相关的操作
- **主要特性**:
  - 图像预处理（尺寸调整、格式转换）
  - 非极大值抑制(NMS)应用
  - 检测结果坐标缩放

### 4. 可视化模块 (Visualizer)
- **文件**: `include/Visualizer.h`, `src/Visualizer.cpp`
- **功能**: 负责结果可视化
- **主要特性**:
  - 检测结果绘制（边界框、关键点、标签）
  - 性能信息显示
  - 颜色和类别名称映射

### 5. 性能监控模块 (PerformanceMonitor)
- **文件**: `include/PerformanceMonitor.h`, `src/PerformanceMonitor.cpp`
- **功能**: 监控和统计性能指标
- **主要特性**:
  - 执行时间统计
  - FPS计算
  - 推理时间监控
  - 性能报告生成

## 文件结构

```
c++/
├── include/                    # 头文件目录
│   ├── Config.h               # 配置类声明
│   ├── Detector.h             # 检测器类声明
│   ├── ImageProcessor.h       # 图像处理类声明
│   ├── Visualizer.h           # 可视化类声明
│   └── PerformanceMonitor.h   # 性能监控类声明
├── src/                       # 源文件目录
│   ├── main.cpp               # 原始单文件版本
│   ├── main_modular.cpp       # 模块化主程序
│   ├── Config.cpp             # 配置类实现
│   ├── Detector.cpp           # 检测器类实现
│   ├── ImageProcessor.cpp     # 图像处理类实现
│   ├── Visualizer.cpp         # 可视化类实现
│   └── PerformanceMonitor.cpp # 性能监控类实现
├── CMakeLists.txt             # CMake构建配置
└── README_模块化重构.md       # 本文档
```

## 构建和运行

### 构建项目
```bash
# 在c++目录下
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### 运行程序
```bash
# 运行原始版本
./Debug/original_main.exe

# 运行模块化版本
./Debug/modular_main.exe
```

## 模块化优势

### 1. 可维护性
- 每个模块职责单一，便于理解和修改
- 代码结构清晰，易于定位问题
- 模块间接口明确，降低耦合度

### 2. 可扩展性
- 新增功能只需添加新模块或扩展现有模块
- 可以轻松替换某个模块的实现
- 支持插件化架构

### 3. 可重用性
- 模块可以在其他项目中重用
- 支持单元测试，提高代码质量
- 便于团队协作开发

### 4. 配置灵活性
- 支持配置文件管理参数
- 运行时参数调整
- 不同场景的配置预设

## 使用示例

### 基本使用
```cpp
#include "Config.h"
#include "Detector.h"
#include "ImageProcessor.h"
#include "Visualizer.h"
#include "PerformanceMonitor.h"

int main() {
    // 初始化配置
    Config config;
    
    // 初始化检测器
    Detector detector(config.getModelPath(), config.getDevice());
    
    // 初始化其他组件
    ImageProcessor processor;
    Visualizer visualizer;
    PerformanceMonitor monitor;
    
    // 执行检测
    cv::Mat frame = cv::imread("test.jpg");
    std::vector<DetectionResult> results = detector.detect(frame);
    
    // 可视化结果
    visualizer.drawDetections(frame, results);
    cv::imshow("Result", frame);
    cv::waitKey(0);
    
    return 0;
}
```

### 自定义配置
```cpp
Config config;
config.setModelPath("path/to/model.onnx");
config.setDevice("GPU");
config.setDetectColor(0); // 检测红色
config.setConfidenceThreshold(0.5f);
config.saveToFile("config.txt");
```

## 扩展指南

### 添加新模块
1. 在`include/`目录下创建头文件
2. 在`src/`目录下创建实现文件
3. 在`CMakeLists.txt`中添加新源文件
4. 在主程序中集成新模块

### 修改现有模块
1. 保持公共接口不变
2. 在实现文件中进行修改
3. 更新相关文档
4. 运行测试确保功能正常

## 注意事项

1. **依赖管理**: 确保所有必要的库都已正确安装和配置
2. **内存管理**: 注意OpenCV和OpenVINO对象的内存管理
3. **异常处理**: 各模块都包含适当的异常处理机制
4. **性能优化**: 可以根据需要进一步优化各模块的性能

## 版本历史

- **v1.0**: 原始单文件版本
- **v2.0**: 模块化重构版本
  - 分离为5个核心模块
  - 改进的错误处理
  - 增强的配置管理
  - 更好的性能监控 