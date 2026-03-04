#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/float32.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <vector>
#include <map>
#include <filesystem>

class DetectNode : public rclcpp::Node
{
public:
    DetectNode() : Node("detect_node")
    {
        // 声明参数：模板路径
        this->declare_parameter<std::string>("templates_path", "");
        std::string templates_path = this->get_parameter("templates_path").as_string();
        
        // 加载数字模板 (0-9)
        loadTemplates(templates_path);

        // 订阅相机图像
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/image_raw", 10, std::bind(&DetectNode::image_callback, this, std::placeholders::_1));

        // 发布识别结果
        digit_pub_ = this->create_publisher<std_msgs::msg::Int32>("/digit_class", 10);
        score_pub_ = this->create_publisher<std_msgs::msg::Float32>("/digit_score", 10);

        RCLCPP_INFO(this->get_logger(), "检测节点已启动，模板路径：%s", templates_path.c_str());
    }

private:
    // 加载模板图片
    void loadTemplates(const std::string& path) {
        for (int i = 0; i <= 9; ++i) {
            std::string file_path = path + "/" + std::to_string(i) + ".jpg";
            cv::Mat img = cv::imread(file_path, cv::IMREAD_GRAYSCALE);
            if (!img.empty()) {
                templates_[i] = img;
                RCLCPP_INFO(this->get_logger(), "加载模板 %d 成功", i);
            } else {
                RCLCPP_WARN(this->get_logger(), "无法加载模板 %s", file_path.c_str());
            }
        }
    }

    // 图像回调函数
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        // 1. ROS图像转OpenCV图像
        cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
// ===================================================================
        // cv::Mat imgHSV;
        // cv::cvtColor(frame, imgHSV, cv::COLOR_BGR2HSV);

        // int hmin = 2,smin = 102,vmin = 0;
        // int hmax = 16,smax = 255,vmax = 255;
      
        // cv::Mat mask;
        // cv::Scalar lower(hmin,smin,vmin);
        // cv::Scalar upper(hmax,smax,vmax);
        // cv::inRange(imgHSV,lower,upper,mask);
// ===================================================================

        
        int detected_digit = -1;
        double best_score = 0.0;

        for (const auto& [digit, templ] : templates_) {
            cv::Mat result;
            // cv::matchTemplate(mask, templ, result, cv::TM_CCOEFF_NORMED);
            cv::matchTemplate(gray, templ, result, cv::TM_CCOEFF_NORMED);
            double min_val, max_val;
            cv::minMaxLoc(result, &min_val, &max_val);
            if (max_val > best_score) {
                best_score = max_val;
                detected_digit = digit;
            }
        }

        // 3. 设置置信度阈值（例如0.5）
        const double CONFIDENCE_THRESHOLD = 0.5;
        if (best_score < CONFIDENCE_THRESHOLD) {
            detected_digit = -1;  // 识别失败
        }

        // 4. 发布结果
        std_msgs::msg::Int32 digit_msg;
        digit_msg.data = detected_digit;
        digit_pub_->publish(digit_msg);

        std_msgs::msg::Float32 score_msg;
        score_msg.data = best_score;
        score_pub_->publish(score_msg);

        RCLCPP_INFO(this->get_logger(), "识别结果：%d, 置信度：%.2f", detected_digit, best_score);
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr digit_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr score_pub_;
    std::map<int, cv::Mat> templates_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DetectNode>());
    rclcpp::shutdown();
    return 0;
}