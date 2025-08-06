#include "../include/Visualizer.h"
#include "../ncnn/cpp/include/STrack.h"
#include <sstream>
#include <iomanip>

void Visualizer::drawDetections(cv::Mat& image, const std::vector<DetectionResult>& detections) {
    for (const auto& detection : detections) {
        // 构建标签
        std::string colorName = getColorName(detection.colorId);
        std::string className = getClassName(detection.classId);
        std::ostringstream label;
        label << "Conf: " << std::fixed << std::setprecision(2) << detection.confidence
              << ", Color: " << colorName
              << ", Class: " << className;
        
        // 绘制边界框
        drawBoundingBox(image, detection.boundingBox, label.str());
        
        // 绘制关键点
        drawLandmarks(image, detection.landmarks);
    }
}

void Visualizer::drawTracks(cv::Mat& image, const std::vector<STrack>& tracks) {
    for (const auto& track : tracks) {
        // 获取跟踪边界框
        std::vector<float> tlbr = track.tlbr;
        cv::Rect trackBox(tlbr[0], tlbr[1], tlbr[2] - tlbr[0], tlbr[3] - tlbr[1]);
        
        // 构建跟踪标签
        std::ostringstream label;
        label << "ID: " << track.track_id;
        
        // 获取跟踪ID对应的颜色
        cv::Scalar trackColor = getTrackColor(track.track_id);
        
        // 绘制跟踪边界框
        drawBoundingBox(image, trackBox, label.str(), trackColor);
        
        // 绘制跟踪状态指示器
        std::string stateText;
        cv::Scalar stateColor;
        switch (track.state) {
            case 0: // New
                stateText = "NEW";
                stateColor = cv::Scalar(0, 255, 0); // 绿色
                break;
            case 1: // Tracked
                stateText = "TRACKED";
                stateColor = cv::Scalar(255, 0, 0); // 蓝色
                break;
            case 2: // Lost
                stateText = "LOST";
                stateColor = cv::Scalar(0, 0, 255); // 红色
                break;
            case 3: // Removed
                stateText = "REMOVED";
                stateColor = cv::Scalar(128, 128, 128); // 灰色
                break;
            default:
                stateText = "UNKNOWN";
                stateColor = cv::Scalar(255, 255, 255); // 白色
                break;
        }
        
        // 在边界框上方绘制状态信息
        cv::putText(image, stateText, cv::Point(trackBox.x, trackBox.y - 25), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, stateColor, 1);
    }
}

void Visualizer::drawPerformanceInfo(cv::Mat& image, double fps, double inferenceTime) {
    std::ostringstream fpsText;
    fpsText << "FPS: " << std::fixed << std::setprecision(1) << fps;
    
    std::ostringstream timeText;
    timeText << "Inference: " << std::fixed << std::setprecision(1) << inferenceTime << "ms";
    
    // 绘制FPS信息
    cv::putText(image, fpsText.str(), cv::Point(10, 30), 
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
    
    // 绘制推理时间信息
    cv::putText(image, timeText.str(), cv::Point(10, 70), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
}

void Visualizer::drawLandmarks(cv::Mat& image, const std::vector<cv::Point2f>& landmarks) {
    for (const auto& landmark : landmarks) {
        cv::circle(image, landmark, 2, cv::Scalar(255, 0, 255), -1);
    }
    cv::Point point1(landmarks[0].x, landmarks[0].y);
    cv::Point point2(landmarks[2].x, landmarks[2].y);

    cv::Point center = (point1 + point2) * 0.5f;
    cv::circle(image, center, 2, cv::Scalar(0, 0, 255), -1);
}

void Visualizer::drawBoundingBox(cv::Mat& image, const cv::Rect& box, const std::string& label, 
                                const cv::Scalar& color) {
    // 绘制矩形框
    cv::rectangle(image, box, color, 1);

    
    // 绘制标签文字
    cv::putText(image, label, cv::Point(box.x, box.y - 5), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

std::string Visualizer::getColorName(int colorId) {
    switch (colorId) {
        case 0: return "Red";
        case 1: return "Blue";
        default: return "Unknown";
    }
}

std::string Visualizer::getClassName(int classId) {
    switch (classId) {
        case 0: return "0";
        case 1: return "1";
        case 2: return "2";
        case 3: return "3";
        case 4: return "4";
        case 5: return "5";
        case 6: return "6";
        case 7: return "7";
        case 8: return "8";
        case 9: return "9";
        default: return "Unknown";
    }
} 

cv::Scalar Visualizer::getTrackColor(int trackId) {
    // 预定义的颜色数组，为不同的跟踪ID分配不同颜色
    static const cv::Scalar colors[] = {
        cv::Scalar(255, 0, 0),    // 蓝色
        cv::Scalar(0, 255, 0),    // 绿色
        cv::Scalar(0, 0, 255),    // 红色
        cv::Scalar(255, 255, 0),  // 青色
        cv::Scalar(255, 0, 255),  // 洋红色
        cv::Scalar(0, 255, 255),  // 黄色
        cv::Scalar(128, 0, 0),    // 深蓝色
        cv::Scalar(0, 128, 0),    // 深绿色
        cv::Scalar(0, 0, 128),    // 深红色
        cv::Scalar(128, 128, 0),  // 橄榄色
        cv::Scalar(128, 0, 128),  // 紫色
        cv::Scalar(0, 128, 128),  // 青色
        cv::Scalar(64, 64, 64),   // 深灰色
        cv::Scalar(192, 192, 192), // 浅灰色
        cv::Scalar(255, 128, 0),  // 橙色
        cv::Scalar(128, 255, 0),  // 黄绿色
        cv::Scalar(0, 128, 255),  // 天蓝色
        cv::Scalar(255, 0, 128),  // 粉红色
        cv::Scalar(128, 0, 255),  // 紫罗兰色
        cv::Scalar(0, 255, 128)   // 春绿色
    };
    
    const int numColors = sizeof(colors) / sizeof(colors[0]);
    return colors[trackId % numColors];
} 