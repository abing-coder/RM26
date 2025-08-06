#pragma once

#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>
#include <vector>
#include <memory>

// 检测结果结构体
struct DetectionResult {
    cv::Rect boundingBox;                           // 边界框
    float confidence;                               // 置信度
    int classId;                                    // 类别ID
    int colorId;                                    // 颜色ID
    std::vector<cv::Point2f> landmarks;             // 关键点坐标
};

// 检测器类
class Detector {
public:
    Detector(const std::string& modelPath, const std::string& device = "CPU");
    ~Detector() = default;

    // 执行检测
    std::vector<DetectionResult> detect(const cv::Mat& frame, int detectColor = 1);
    
    // 获取模型输入尺寸
    cv::Size getInputSize() const;

private:
    // 初始化模型
    void initializeModel(const std::string& modelPath, const std::string& device);
    
    // 预处理图像
    std::vector<float> preprocessImage(const cv::Mat& frame);
    
    // 后处理检测结果
    std::vector<DetectionResult> postprocessResults(const float* outputData, 
                                                   const std::vector<int>& outputShape,
                                                   int detectColor);
    
    // 计算边界框
    cv::Rect computeBoundingBox(const std::vector<cv::Point2f>& points);
    
    // Sigmoid激活函数
    inline float sigmoid(float x) const {
        return 1.0f / (1.0f + std::exp(-x));
    }

private:
    ov::Core core_;                                 // OpenVINO核心对象
    ov::CompiledModel compiledModel_;               // 编译后的模型
    ov::InferRequest inferRequest_;                 // 推理请求
    ov::Output<const ov::Node> inputPort_;          // 输入端口
    ov::Output<const ov::Node> outputPort_;         // 输出端口
    cv::Size inputSize_;                            // 模型输入尺寸
}; 