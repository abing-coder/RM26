#include "../include/ImageProcessor.h"
#include <opencv2/dnn.hpp>

cv::Mat ImageProcessor::preprocessForInference(const cv::Mat& frame, const cv::Size& targetSize) {
    cv::Mat resized;
    cv::resize(frame, resized, targetSize);
    return resized;
}

std::vector<int> ImageProcessor::applyNMS(const std::vector<cv::Rect>& boxes, 
                                         const std::vector<float>& confidences,
                                         float confidenceThreshold,
                                         float nmsThreshold) {
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);
    return indices;
}

std::vector<DetectionResult> ImageProcessor::scaleResultsToOriginal(const std::vector<DetectionResult>& results,
                                                                   const cv::Size& originalSize,
                                                                   const cv::Size& processedSize) {
    std::vector<DetectionResult> scaledResults;
    
    float scaleX = static_cast<float>(originalSize.width) / processedSize.width;
    float scaleY = static_cast<float>(originalSize.height) / processedSize.height;
    
    for (const auto& result : results) {
        DetectionResult scaledResult = result;
        
        // 缩放边界框
        scaledResult.boundingBox.x = static_cast<int>(result.boundingBox.x * scaleX);
        scaledResult.boundingBox.y = static_cast<int>(result.boundingBox.y * scaleY);
        scaledResult.boundingBox.width = static_cast<int>(result.boundingBox.width * scaleX);
        scaledResult.boundingBox.height = static_cast<int>(result.boundingBox.height * scaleY);
        
        // 缩放关键点
        for (auto& landmark : scaledResult.landmarks) {
            landmark.x *= scaleX;
            landmark.y *= scaleY;
        }
        
        scaledResults.push_back(scaledResult);
    }
    
    return scaledResults;
}

std::vector<float> ImageProcessor::convertToTensor(const cv::Mat& image) {
    // 转换颜色空间
    cv::Mat rgb;
    cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
    
    // 归一化
    rgb.convertTo(rgb, CV_32F, 1.0 / 255.0);
    
    // 分离通道
    std::vector<cv::Mat> channels(3);
    cv::split(rgb, channels);
    
    // 重新排列为NCHW格式
    std::vector<float> tensor(3 * image.rows * image.cols);
    for (int c = 0; c < 3; ++c) {
        memcpy(tensor.data() + c * image.rows * image.cols, 
               channels[c].data, 
               image.rows * image.cols * sizeof(float));
    }
    
    return tensor;
} 