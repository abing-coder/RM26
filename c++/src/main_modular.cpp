#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <vector>

// 包含自定义模块头文件
#include "../include/Config.h"
#include "../include/Detector.h"
#include "../include/ImageProcessor.h"
#include "../include/Visualizer.h"
#include "../include/PerformanceMonitor.h"

// 包含BYTETracker相关头文件
#include "../ncnn/cpp/include/BYTETracker.h"
#include "../ncnn/cpp/include/STrack.h"

int main() {
    try {
        // ==================== 初始化配置 ====================
        Config config;
        
        // config.printConfig();
        
        // ==================== 初始化检测器 ====================
        Detector detector(config.getModelPath(), config.getDevice());
        
        // ==================== 预处理 =========================
        ImageProcessor imageProcessor;
        Visualizer visualizer;
        PerformanceMonitor performanceMonitor;
        
        // ==================== 初始化BYTETracker跟踪器 ====================
        BYTETracker tracker(60, 60); // 帧率30fps，跟踪缓冲区30帧
        
        // ==================== 初始化视频捕获 ====================
        cv::VideoCapture cap(config.getVideoPath());
        if (!cap.isOpened()) {
            std::cerr << "Cannot open video file: " << config.getVideoPath() << std::endl;
            return -1;
        }
        
        // ==================== 开始性能监控 ====================
        performanceMonitor.start();
        
        // ==================== 主处理循环 ====================
        while (true) {
            cv::Mat frame;
            
            // 读取视频帧
            if (!cap.read(frame)) {
                std::cout << "Video ended or cannot read frame" << std::endl;
                break;
            }
            
            // 记录推理开始时间
            auto inferStart = std::chrono::high_resolution_clock::now();
            
            // ==================== 执行检测 ====================
            std::vector<DetectionResult> detections = detector.detect(frame, config.getDetectColor());
            
            // 记录推理结束时间
            auto inferEnd = std::chrono::high_resolution_clock::now();
            double inferenceTime = std::chrono::duration<double, std::milli>(inferEnd - inferStart).count();
            
            // ==================== 应用NMS ====================
            std::vector<cv::Rect> boxes;
            std::vector<float> confidences;
            
            for (const auto& detection : detections) {
                boxes.push_back(detection.boundingBox);
                confidences.push_back(detection.confidence);
            }
            
            std::vector<int> indices = imageProcessor.applyNMS(boxes, confidences, 
                                                              config.getConfidenceThreshold(), 
                                                              config.getNMSThreshold());
            
            // ==================== 筛选检测结果 ====================
            std::vector<DetectionResult> filteredDetections;
            for (int idx : indices) {
                filteredDetections.push_back(detections[idx]);
            }
            
            // ==================== 调整结果到原始尺寸 ====================
            cv::Size originalSize = frame.size();
            cv::Size processedSize = detector.getInputSize();
            std::vector<DetectionResult> scaledDetections = imageProcessor.scaleResultsToOriginal(
                filteredDetections, originalSize, processedSize);
            
            // ==================== 转换为BYTETracker需要的Object格式 ====================
            std::vector<Object> objects;
            for (const auto& detection : scaledDetections) {
                Object obj;
                obj.rect = detection.boundingBox;
                obj.label = detection.classId;
                obj.prob = detection.confidence;
                objects.push_back(obj);
            }
            
            // ==================== 执行跟踪 ====================
            std::vector<STrack> tracks = tracker.update(objects);
            
            // ==================== 可视化结果 ====================
            cv::Mat displayFrame = frame.clone();
            
            // 绘制检测结果
            visualizer.drawDetections(displayFrame, scaledDetections);
            
            // 绘制跟踪结果
            visualizer.drawTracks(displayFrame, tracks);
            
            // 计算并绘制性能信息
            double fps = 1000.0 / inferenceTime;
            visualizer.drawPerformanceInfo(displayFrame, fps, inferenceTime);
            
            // ==================== 更新性能统计 ====================
            performanceMonitor.recordInferenceTime(inferenceTime);
            performanceMonitor.incrementFrameCount();
            
            // ==================== 显示结果 ====================
            cv::imshow("OpenVINO Detection Result", displayFrame);
        
            
            // 检查按键：按'q'退出
            if (cv::waitKey(1) == 'q') break;
        }
        
        // ==================== 程序结束处理 ====================
        performanceMonitor.end();
        
        // 释放资源
        cap.release();
        cv::destroyAllWindows();
        
        // ==================== 输出性能统计 ====================
        performanceMonitor.printStatistics();
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Program execution error: " << e.what() << std::endl;
        return -1;
    }
} 