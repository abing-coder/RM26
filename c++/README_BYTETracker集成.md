# BYTETracker集成说明

## 概述

本项目已成功集成了BYTETracker多目标跟踪算法，用于在检测结果的基础上进行目标跟踪。

## 集成内容

### 1. 核心文件
- `ncnn/cpp/include/BYTETracker.h` - BYTETracker主类头文件
- `ncnn/cpp/include/STrack.h` - 跟踪目标类头文件
- `ncnn/cpp/include/kalmanFilter.h` - 卡尔曼滤波器头文件
- `ncnn/cpp/include/lapjv.h` - 线性分配算法头文件
- `ncnn/cpp/src/BYTETracker.cpp` - BYTETracker实现
- `ncnn/cpp/src/STrack.cpp` - 跟踪目标实现
- `ncnn/cpp/src/kalmanFilter.cpp` - 卡尔曼滤波器实现
- `ncnn/cpp/src/lapjv.cpp` - 线性分配算法实现
- `ncnn/cpp/src/utils.cpp` - 工具函数实现

### 2. 主程序修改
- `src/main_modular.cpp` - 添加了BYTETracker初始化和跟踪处理
- `include/Visualizer.h` - 添加了绘制跟踪结果的方法声明
- `src/Visualizer.cpp` - 添加了绘制跟踪结果的方法实现

### 3. 构建配置
- `CMakeLists.txt` - 添加了BYTETracker相关源文件的编译配置

## 功能特性

### 1. 多目标跟踪
- 支持多个目标的同时跟踪
- 自动分配唯一的跟踪ID
- 处理目标的出现、消失和重新出现

### 2. 跟踪状态管理
- **NEW**: 新检测到的目标
- **TRACKED**: 正在跟踪的目标
- **LOST**: 暂时丢失的目标
- **REMOVED**: 已移除的目标

### 3. 可视化增强
- 不同跟踪ID使用不同颜色显示
- 显示跟踪状态信息
- 显示跟踪ID标签

## 使用方法

### 1. 编译项目
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### 2. 运行主程序
```bash
./modular_main.exe
```

### 3. 运行测试程序
```bash
./test_bytetracker.exe
```

## 核心代码示例

### 初始化BYTETracker
```cpp
// 初始化BYTETracker跟踪器
BYTETracker tracker(30, 30); // 帧率30fps，跟踪缓冲区30帧
```

### 转换检测结果
```cpp
// 转换为BYTETracker需要的Object格式
std::vector<Object> objects;
for (const auto& detection : scaledDetections) {
    Object obj;
    obj.rect = detection.boundingBox;
    obj.label = detection.classId;
    obj.prob = detection.confidence;
    objects.push_back(obj);
}
```

### 执行跟踪
```cpp
// 执行跟踪
std::vector<STrack> tracks = tracker.update(objects);
```

### 可视化跟踪结果
```cpp
// 绘制跟踪结果
visualizer.drawTracks(displayFrame, tracks);
```

## 参数配置

### BYTETracker参数
- `frame_rate`: 视频帧率（默认30fps）
- `track_buffer`: 跟踪缓冲区大小（默认30帧）
- `track_thresh`: 跟踪阈值（默认0.5）
- `high_thresh`: 高置信度阈值（默认0.6）
- `match_thresh`: 匹配阈值（默认0.8）

### 可视化参数
- 跟踪ID颜色：20种预定义颜色循环使用
- 状态颜色：
  - NEW: 绿色
  - TRACKED: 蓝色
  - LOST: 红色
  - REMOVED: 灰色

## 性能优化

1. **内存管理**: 自动清理过期的跟踪目标
2. **计算优化**: 使用高效的IoU距离计算
3. **状态预测**: 卡尔曼滤波器预测目标位置

## 注意事项

1. 确保检测结果的边界框格式正确（cv::Rect）
2. 跟踪器需要连续的帧输入才能正常工作
3. 跟踪ID在目标重新出现时可能会改变
4. 建议在检测结果质量较好的情况下使用跟踪器

## 故障排除

### 常见问题
1. **编译错误**: 检查OpenCV和BYTETracker头文件路径
2. **运行时错误**: 确保所有依赖库正确链接
3. **跟踪效果差**: 调整检测阈值和跟踪参数

### 调试方法
1. 运行测试程序验证集成
2. 检查跟踪状态输出
3. 调整可视化参数观察效果 