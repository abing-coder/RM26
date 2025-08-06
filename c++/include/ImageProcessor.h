#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "Detector.h"

// 图像处理类
class ImageProcessor {
public:
    ImageProcessor() = default;
    ~ImageProcessor() = default;

    // 预处理图像用于模型输入
    cv::Mat preprocessForInference(const cv::Mat& frame, const cv::Size& targetSize);
    
    // 应用非极大值抑制
    std::vector<int> applyNMS(const std::vector<cv::Rect>& boxes, 
                              const std::vector<float>& confidences,
                              float confidenceThreshold = 0.45f,
                              float nmsThreshold = 0.45f);
    
    // 调整检测结果到原始图像尺寸
    std::vector<DetectionResult> scaleResultsToOriginal(const std::vector<DetectionResult>& results,
                                                        const cv::Size& originalSize,
                                                        const cv::Size& processedSize);

private:
    // 将图像转换为模型输入格式
    std::vector<float> convertToTensor(const cv::Mat& image);
}; 