#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("one_shot_publisher");
  auto publisher = node->create_publisher<std_msgs::msg::String>("topic", 10);

  // subscriber discoveryのため少し待つ
  rclcpp::sleep_for(500ms);

  std_msgs::msg::String message;
  message.data = "Hello, ROS2! Publish once.";

  RCLCPP_INFO(node->get_logger(), "Publishing: '%s'", message.data.c_str());
  publisher->publish(message);

  // 送信処理が流れるよう少し待つ
  rclcpp::sleep_for(200ms);

  rclcpp::shutdown();
  return 0;
}