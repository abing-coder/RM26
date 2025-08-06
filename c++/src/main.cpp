#include <iostream>      // 输入输出流
#include <opencv2/opencv.hpp>  // OpenCV计算机视觉库
#include <openvino/openvino.hpp>  // OpenVINO推理引擎
#include <vector>        // 动态数组容器
#include <chrono>        // 时间相关功能
#include <ctime>         // C风格时间函数


inline float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}


cv::Rect compute_bounding_box(const std::vector<cv::Point2f>& points) {
    // 初始化最小最大值为第一个点的坐标
    float min_x = points[0].x, max_x = points[0].x;
    float min_y = points[0].y, max_y = points[0].y;
    
    // 遍历所有关键点，更新边界框的坐标范围
    for (const auto& p : points) {
        if (p.x < min_x) min_x = p.x;  // 更新最小x坐标
        if (p.x > max_x) max_x = p.x;  // 更新最大x坐标
        if (p.y < min_y) min_y = p.y;  // 更新最小y坐标
        if (p.y > max_y) max_y = p.y;  // 更新最大y坐标
    }
    
    // 返回包含所有关键点的矩形边界框
    return cv::Rect(cv::Point2f(min_x, min_y), cv::Point2f(max_x, max_y));
}


int main() {
    // ==================== 配置参数 ====================
    std::string model_path = "D:/RM26-DetectionModel/model/0708.onnx";  // ONNX模型文件路径
    std::string video_path = "D:/RM26-DetectionModel/8radps.avi";       // 输入视频文件路径
    std::string device = "CPU";        // 推理设备类型（CPU/GPU/VPU等）
    int detect_color = 1;              // 检测颜色：0表示红色，1表示蓝色

    // ==================== OpenVINO初始化 ====================
    ov::Core core;  // 创建OpenVINO核心对象
    std::shared_ptr<ov::Model> model = core.read_model(model_path);  // 读取ONNX模型
    ov::CompiledModel compiled_model = core.compile_model(model, device);  // 编译模型到指定设备
    ov::InferRequest infer_request = compiled_model.create_infer_request();  // 创建推理请求
    auto input_port = compiled_model.input();   // 获取模型输入端口信息
    auto output_port = compiled_model.output(); // 获取模型输出端口信息

    // ==================== 视频文件初始化 ====================
    cv::VideoCapture cap(video_path);  // 打开视频文件
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video file: " << video_path << std::endl;
        return -1;  // 视频打开失败，返回错误码
    }

    // ==================== 性能统计变量 ====================
    int total_frames = 0;              // 总处理帧数
    double total_inference_time = 0.0; // 总推理时间（秒）

    // ==================== 开始计时 ====================
    auto start = std::chrono::high_resolution_clock::now();

    // ==================== 主处理循环 ====================
    while (true) {
        cv::Mat frame;  // 存储当前帧图像
        
        // 读取视频帧
        if (!cap.read(frame)) {
            std::cout << "Video ended or cannot read frame" << std::endl;
            break;  // 视频结束或无法读取帧，退出循环
        }
        
        auto pre_start = std::chrono::high_resolution_clock::now();  // 预处理开始时间
        
        // ==================== 图像预处理 ====================
        cv::Mat resized;  // 存储调整大小后的图像
        cv::resize(frame, resized, cv::Size(640, 640));  // 将图像调整为640x640大小
        
        cv::Mat rgb;  // 存储RGB格式图像
        cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);  // BGR转RGB颜色空间
        
        // 归一化：将像素值从[0,255]转换为[0,1]
        rgb.convertTo(rgb, CV_32F, 1.0 / 255.0);
        
        // 分离RGB三个通道
        std::vector<cv::Mat> channels(3);
        cv::split(rgb, channels);
        
        // 将图像数据重新排列为NCHW格式（批次、通道、高度、宽度）
        std::vector<float> input_tensor(3 * 640 * 640);
        for (int c = 0; c < 3; ++c) {
            // 将每个通道的数据复制到对应的位置
            memcpy(input_tensor.data() + c * 640 * 640, channels[c].data, 640 * 640 * sizeof(float));
        }
        
        // ==================== 模型推理 ====================
        // 创建输入张量
        ov::Tensor input = ov::Tensor(input_port.get_element_type(), {1, 3, 640, 640}, input_tensor.data());
        infer_request.set_input_tensor(input);  // 设置输入张量
        
        auto infer_start = std::chrono::high_resolution_clock::now();  // 推理开始时间
        infer_request.infer();  // 执行推理
        auto infer_end = std::chrono::high_resolution_clock::now();    // 推理结束时间
        
        // 计算推理时间（毫秒）
        double infer_time = std::chrono::duration<double, std::milli>(infer_end - infer_start).count();
        total_inference_time += infer_time / 1000.0;  // 累加总推理时间（秒）
        total_frames++;  // 帧数计数加1
        
        // ==================== 后处理 - 解析模型输出 ====================
        ov::Tensor output = infer_request.get_output_tensor();  // 获取输出张量
        float* out_data = output.data<float>();  // 获取输出数据指针
        auto out_shape = output.get_shape();  // 获取输出形状：(1, 25200, 22)
        
        int num_boxes = out_shape[1];   // 检测框数量：25200
        int num_attrs = out_shape[2];   // 每个检测框的属性数量：22
        
        // 存储检测结果的容器
        std::vector<cv::Rect> boxes;                    // 边界框坐标
        std::vector<float> confidences;                 // 置信度分数
        std::vector<int> class_ids, color_ids;         // 类别ID和颜色ID
        std::vector<std::vector<cv::Point2f>> all_landmarks;  // 关键点坐标
        
        // 遍历所有检测框
        for (int i = 0; i < num_boxes; ++i) {
            float* det = out_data + i * num_attrs;  // 当前检测框的数据指针
            
            // 计算置信度（使用sigmoid激活函数）
            float confidence = sigmoid(det[8]);
            if (confidence < 0.45f) continue;  // 置信度阈值过滤
            
            // 颜色分类：找到概率最大的颜色类别
            int color_id = std::max_element(det + 8, det + 14) - (det + 8);
            
            // 目标分类：找到概率最大的目标类别
            int class_id = std::max_element(det + 13, det + 23) - (det + 13);
            
            // 颜色过滤：只保留指定颜色的检测结果
            if ((detect_color == 0 && color_id == 1) || (detect_color == 1 && color_id == 0)) continue;
            
            // 提取关键点坐标（前8个值：4个关键点的x,y坐标）
            std::vector<cv::Point2f> points;
            for (int j = 0; j < 4; ++j) {
                points.emplace_back(det[j * 2], det[j * 2 + 1]);  // 添加关键点坐标
            }
            
            if (points.size() != 4) continue;  // 确保有4个关键点
            
            // 计算边界框
            cv::Rect rect = compute_bounding_box(points);
            
            // 保存检测结果
            boxes.push_back(rect);
            confidences.push_back(confidence);
            class_ids.push_back(class_id);
            color_ids.push_back(color_id);
            all_landmarks.push_back(points);
        }
        
        // ==================== 非极大值抑制(NMS) ====================
        std::vector<int> indices;  // 存储NMS后保留的检测框索引
        cv::dnn::NMSBoxes(boxes, confidences, 0.45f, 0.45f, indices);
        
        // ==================== 结果可视化 ====================
        for (int idx : indices) {
            const auto& rect = boxes[idx];           // 边界框
            float confidence = confidences[idx];     // 置信度
            int color_id = color_ids[idx];          // 颜色ID
            int class_id = class_ids[idx];          // 类别ID
            const auto& landmarks = all_landmarks[idx];  // 关键点
            
            // 绘制边界框（绿色，线宽1）
            cv::rectangle(resized, rect, cv::Scalar(0, 255, 0), 1);
            
            // 绘制关键点（紫色，半径2，实心）
            for (const auto& pt : landmarks) {
                cv::circle(resized, pt, 2, cv::Scalar(255, 0, 255), -1);
            }
            
            // 绘制标签文本
            char label[64];
            snprintf(label, sizeof(label), "Conf: %.2f, Color: %d, Label: %d", confidence, color_id, class_id);
            cv::putText(resized, label, cv::Point(rect.x, rect.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 5), 1);
        }
        
        // ==================== 显示性能信息 ====================
        double fps = 1000.0 / infer_time;  // 计算当前帧的FPS
        cv::putText(resized, cv::format("FPS: %.1f", fps), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        
        // 显示处理后的图像
        cv::imshow("OpenVINO Detection", resized);
        
        // 检查按键：按'q'退出
        if (cv::waitKey(1) == 'q') break;
    }

    // ==================== 程序结束处理 ====================
    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();

    // 计算总执行时间（毫秒）
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // 输出总执行时间
    std::cout << "Execution time: " << duration / 1000.0 << " seconds" << std::endl;

    // 释放资源
    cap.release();  // 释放视频捕获对象
    cv::destroyAllWindows();  // 关闭所有OpenCV窗口
    
    // ==================== 性能统计输出 ====================
    if (total_frames > 0) {
        double avg_fps = total_frames / total_inference_time;           // 平均FPS
        double avg_infer = (total_inference_time / total_frames) * 1000.0;  // 平均推理时间（毫秒）
        
        std::cout << "\nPerformance statistics:" << std::endl;
        std::cout << "Total frames: " << total_frames << std::endl;
        std::cout << "Average FPS: " << avg_fps << std::endl;
        std::cout << "Average inference time: " << avg_infer << " ms" << std::endl;
    }
    
    return 0;  // 程序正常结束
}
