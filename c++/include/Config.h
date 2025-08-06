#pragma once

#include <string>

// 配置类
class Config {
public:
    Config();
    ~Config() = default;

    // 设置模型路径
    void setModelPath(const std::string& path);
    
    // 设置视频路径
    void setVideoPath(const std::string& path);
    
    // 设置推理设备
    void setDevice(const std::string& device);
    
    // 设置检测颜色
    void setDetectColor(int color);
    
    // 设置置信度阈值
    void setConfidenceThreshold(float threshold);
    
    // 设置NMS阈值
    void setNMSThreshold(float threshold);
    
    // 获取模型路径
    std::string getModelPath() const;
    
    // 获取视频路径
    std::string getVideoPath() const;
    
    // 获取推理设备
    std::string getDevice() const;
    
    // 获取检测颜色
    int getDetectColor() const;
    
    // 获取置信度阈值
    float getConfidenceThreshold() const;
    
    // 获取NMS阈值
    float getNMSThreshold() const;
    
    // 从文件加载配置
    bool loadFromFile(const std::string& filename);
    
    // 保存配置到文件
    bool saveToFile(const std::string& filename) const;
    
    // 打印配置信息
    void printConfig() const;

private:
    std::string modelPath_;           // 模型文件路径
    std::string videoPath_;           // 视频文件路径
    std::string device_;              // 推理设备类型
    int detectColor_;                 // 检测颜色：0=红色，1=蓝色
    float confidenceThreshold_;       // 置信度阈值
    float nmsThreshold_;              // NMS阈值
}; 