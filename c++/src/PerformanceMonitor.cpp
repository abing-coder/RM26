#include "../include/PerformanceMonitor.h"
#include <iomanip>

PerformanceMonitor::PerformanceMonitor() 
    : totalFrames_(0), totalInferenceTime_(0.0), isRunning_(false) {
}

void PerformanceMonitor::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
    isRunning_ = true;
}

void PerformanceMonitor::end() {
    if (isRunning_) {
        endTime_ = std::chrono::high_resolution_clock::now();
        isRunning_ = false;
    }
}

void PerformanceMonitor::recordInferenceTime(double inferenceTime) {
    totalInferenceTime_ += inferenceTime / 1000.0; // 转换为秒
}

void PerformanceMonitor::incrementFrameCount() {
    totalFrames_++;
}

double PerformanceMonitor::getTotalTime() const {
    if (!isRunning_) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime_ - startTime_);
        return duration.count() / 1000.0;
    }
    return 0.0;
}

double PerformanceMonitor::getAverageFPS() const {
    if (totalInferenceTime_ > 0) {
        return totalFrames_ / totalInferenceTime_;
    }
    return 0.0;
}

double PerformanceMonitor::getAverageInferenceTime() const {
    if (totalFrames_ > 0) {
        return (totalInferenceTime_ / totalFrames_) * 1000.0; // 转换为毫秒
    }
    return 0.0;
}

int PerformanceMonitor::getTotalFrames() const {
    return totalFrames_;
}

void PerformanceMonitor::printStatistics() const {
    std::cout << "\n=== Performance Statistics ===" << std::endl;
    std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << getTotalTime() << " seconds" << std::endl;
    std::cout << "Total frames processed: " << totalFrames_ << std::endl;
    std::cout << "Average FPS: " << std::fixed << std::setprecision(1) << getAverageFPS() << std::endl;
    std::cout << "Average inference time: " << std::fixed << std::setprecision(1) << getAverageInferenceTime() << " ms" << std::endl;
    std::cout << "===============================" << std::endl;
}

void PerformanceMonitor::reset() {
    totalFrames_ = 0;
    totalInferenceTime_ = 0.0;
    isRunning_ = false;
} 