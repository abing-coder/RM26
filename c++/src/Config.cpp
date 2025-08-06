#include "../include/Config.h"
#include <iostream>
#include <fstream>

Config::Config() 
    : modelPath_("D:/RM26-DetectionModel/model/0708.onnx"),
      videoPath_("D:/RM26-DetectionModel/8radps.avi"),
      device_("CPU"),
      detectColor_(1),
      confidenceThreshold_(0.45f),
      nmsThreshold_(0.45f) {
}

void Config::setModelPath(const std::string& path) {
    modelPath_ = path;
}

void Config::setVideoPath(const std::string& path) {
    videoPath_ = path;
}

void Config::setDevice(const std::string& device) {
    device_ = device;
}

void Config::setDetectColor(int color) {
    detectColor_ = color;
}

void Config::setConfidenceThreshold(float threshold) {
    confidenceThreshold_ = threshold;
}

void Config::setNMSThreshold(float threshold) {
    nmsThreshold_ = threshold;
}

std::string Config::getModelPath() const {
    return modelPath_;
}

std::string Config::getVideoPath() const {
    return videoPath_;
}

std::string Config::getDevice() const {
    return device_;
}

int Config::getDetectColor() const {
    return detectColor_;
}

float Config::getConfidenceThreshold() const {
    return confidenceThreshold_;
}

float Config::getNMSThreshold() const {
    return nmsThreshold_;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open configuration file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 去除前后空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "model_path") {
                modelPath_ = value;
            } else if (key == "video_path") {
                videoPath_ = value;
            } else if (key == "device") {
                device_ = value;
            } else if (key == "detect_color") {
                detectColor_ = std::stoi(value);
            } else if (key == "confidence_threshold") {
                confidenceThreshold_ = std::stof(value);
            } else if (key == "nms_threshold") {
                nmsThreshold_ = std::stof(value);
            }
        }
    }
    
    file.close();
    return true;
}

bool Config::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot create configuration file: " << filename << std::endl;
        return false;
    }
    
    file << "model_path=" << modelPath_ << std::endl;
    file << "video_path=" << videoPath_ << std::endl;
    file << "device=" << device_ << std::endl;
    file << "detect_color=" << detectColor_ << std::endl;
    file << "confidence_threshold=" << confidenceThreshold_ << std::endl;
    file << "nms_threshold=" << nmsThreshold_ << std::endl;
    
    file.close();
    return true;
}

void Config::printConfig() const {
    std::cout << "\n=== Configuration Information ===" << std::endl;
    std::cout << "Model path: " << modelPath_ << std::endl;
    std::cout << "Video path: " << videoPath_ << std::endl;
    std::cout << "Inference device: " << device_ << std::endl;
    std::cout << "Detection color: " << (detectColor_ == 0 ? "Red" : "Blue") << std::endl;
    std::cout << "Confidence threshold: " << confidenceThreshold_ << std::endl;
    std::cout << "NMS threshold: " << nmsThreshold_ << std::endl;
    std::cout << "==================================" << std::endl;
} 