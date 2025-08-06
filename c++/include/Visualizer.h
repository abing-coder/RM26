#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "Detector.h"

// 前向声明
class STrack;

// 可视化类
class Visualizer {
public:
    Visualizer() = default;
    ~Visualizer() = default;

    // 绘制检测结果
    void drawDetections(cv::Mat& image, const std::vector<DetectionResult>& detections);
    
    // 绘制跟踪结果
    void drawTracks(cv::Mat& image, const std::vector<STrack>& tracks);
    
    // 绘制性能信息
    void drawPerformanceInfo(cv::Mat& image, double fps, double inferenceTime);
    
    // 绘制关键点
    void drawLandmarks(cv::Mat& image, const std::vector<cv::Point2f>& landmarks);
    
    // 绘制边界框
    void drawBoundingBox(cv::Mat& image, const cv::Rect& box, const std::string& label, 
                        const cv::Scalar& color = cv::Scalar(0, 255, 0));

private:
    // 获取颜色名称
    std::string getColorName(int colorId);
    
    // 获取类别名称
    std::string getClassName(int classId);
    
    // 获取跟踪ID对应的颜色
    cv::Scalar getTrackColor(int trackId);
}; 