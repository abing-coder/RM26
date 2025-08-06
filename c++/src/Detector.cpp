#include "../include/Detector.h"
#include <iostream>
#include <algorithm>

Detector::Detector(const std::string& modelPath, const std::string& device) {
    initializeModel(modelPath, device);
}

void Detector::initializeModel(const std::string& modelPath, const std::string& device) {
    try {
        // 读取模型
        std::shared_ptr<ov::Model> model = core_.read_model(modelPath);
        
        // 编译模型
        compiledModel_ = core_.compile_model(model, device);
        
        // 创建推理请求
        inferRequest_ = compiledModel_.create_infer_request();
        
        // 获取输入输出端口
        inputPort_ = compiledModel_.input();
        outputPort_ = compiledModel_.output();
        
        // 获取输入尺寸
        auto inputShape = inputPort_.get_shape();
        inputSize_ = cv::Size(inputShape[3], inputShape[2]); // NCHW格式
        
        std::cout << "Model loaded successfully. Input size: " << inputSize_ << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing model: " << e.what() << std::endl;
        throw;
    }
}

std::vector<DetectionResult> Detector::detect(const cv::Mat& frame, int detectColor) {
    // 预处理图像
    std::vector<float> inputTensor = preprocessImage(frame);
    
    // 创建输入张量
    ov::Tensor input = ov::Tensor(inputPort_.get_element_type(), 
                                 ov::Shape{1, 3, static_cast<size_t>(inputSize_.height), static_cast<size_t>(inputSize_.width)}, 
                                 inputTensor.data());
    
    // 设置输入
    inferRequest_.set_input_tensor(input);
    
    // 执行推理
    inferRequest_.infer();
    
    // 获取输出
    ov::Tensor output = inferRequest_.get_output_tensor();
    float* outputData = output.data<float>();
    auto outputShape = output.get_shape();
    
    // 后处理结果
    std::vector<int> shape(outputShape.begin(), outputShape.end());
    return postprocessResults(outputData, shape, detectColor);
}

std::vector<float> Detector::preprocessImage(const cv::Mat& frame) {
    // 调整图像尺寸
    cv::Mat resized;
    cv::resize(frame, resized, inputSize_);
    
    // 转换颜色空间
    cv::Mat rgb;
    cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);
    
    // 归一化
    rgb.convertTo(rgb, CV_32F, 1.0 / 255.0);
    
    // 分离通道并重新排列为NCHW格式
    std::vector<cv::Mat> channels(3);
    cv::split(rgb, channels);
    
    std::vector<float> inputTensor(3 * inputSize_.height * inputSize_.width);
    for (int c = 0; c < 3; ++c) {
        memcpy(inputTensor.data() + c * inputSize_.height * inputSize_.width, 
               channels[c].data, 
               inputSize_.height * inputSize_.width * sizeof(float));
    }
    
    return inputTensor;
}

std::vector<DetectionResult> Detector::postprocessResults(const float* outputData, 
                                                         const std::vector<int>& outputShape,
                                                         int detectColor) {
    std::vector<DetectionResult> results;
    
    int numBoxes = outputShape[1];   // 检测框数量
    int numAttrs = outputShape[2];   // 每个检测框的属性数量
    
    for (int i = 0; i < numBoxes; ++i) {
        float* det = const_cast<float*>(outputData) + i * numAttrs;
        
        // 计算置信度
        float confidence = sigmoid(det[8]);
        if (confidence < 0.45f) continue;
        
        // 颜色分类
        int colorId = std::max_element(det + 8, det + 14) - (det + 8);
        
        // 目标分类
        int classId = std::max_element(det + 13, det + 23) - (det + 13);
        
        // 颜色过滤
        if ((detectColor == 0 && colorId == 1) || (detectColor == 1 && colorId == 0)) {
            continue;
        }
        
        // 提取关键点
        std::vector<cv::Point2f> landmarks;
        for (int j = 0; j < 4; ++j) {
            landmarks.emplace_back(det[j * 2], det[j * 2 + 1]);
        }
        
        if (landmarks.size() != 4) continue;
        
        // 计算边界框
        cv::Rect boundingBox = computeBoundingBox(landmarks);
        
        // 创建检测结果
        DetectionResult result;
        result.boundingBox = boundingBox;
        result.confidence = confidence;
        result.classId = classId;
        result.colorId = colorId;
        result.landmarks = landmarks;
        
        results.push_back(result);
    }
    
    return results;
}

cv::Rect Detector::computeBoundingBox(const std::vector<cv::Point2f>& points) {
    if (points.empty()) return cv::Rect();
    
    float minX = points[0].x, maxX = points[0].x;
    float minY = points[0].y, maxY = points[0].y;
    
    for (const auto& p : points) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }
    
    return cv::Rect(cv::Point2f(minX, minY), cv::Point2f(maxX, maxY));
}

cv::Size Detector::getInputSize() const {
    return inputSize_;
} 