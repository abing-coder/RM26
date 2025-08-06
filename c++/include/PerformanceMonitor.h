#pragma once

#include <chrono>
#include <string>
#include <iostream>

// 性能监控类
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;

    // 开始计时
    void start();
    
    // 结束计时
    void end();
    
    // 记录推理时间
    void recordInferenceTime(double inferenceTime);
    
    // 增加帧数计数
    void incrementFrameCount();
    
    // 获取总执行时间（秒）
    double getTotalTime() const;
    
    // 获取平均FPS
    double getAverageFPS() const;
    
    // 获取平均推理时间（毫秒）
    double getAverageInferenceTime() const;
    
    // 获取总帧数
    int getTotalFrames() const;
    
    // 打印性能统计
    void printStatistics() const;
    
    // 重置统计
    void reset();

private:
    std::chrono::high_resolution_clock::time_point startTime_;    // 开始时间
    std::chrono::high_resolution_clock::time_point endTime_;      // 结束时间
    int totalFrames_;                                             // 总帧数
    double totalInferenceTime_;                                   // 总推理时间（秒）
    bool isRunning_;                                              // 是否正在计时
}; 