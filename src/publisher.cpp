#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "lighton_ros2_pubsub/pubsub.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("one_shot_publisher");
  auto publisher = node->create_publisher<std_msgs::msg::String>(TOPIC_NAME, rclcpp::QoS(1).reliable());

  for(int i {0}; i < PUBLISHER_TRY_MAX; i++){
    if (publisher->get_subscription_count() != 0) {
      break;
    }
    RCLCPP_INFO(node->get_logger(), "Waiting for subscriber...");
    rclcpp::sleep_for(PUBLISHER_FIND_WAIT_MSEC);
  }
    
  if (publisher->get_subscription_count() == 0) {
    RCLCPP_ERROR(node->get_logger(), "Subscriber not found.");
    rclcpp::shutdown();
    return -1;
  }

  std_msgs::msg::String message;
  message.data = "Hello, ROS2! Publish once.";

  RCLCPP_INFO(node->get_logger(), "Publish: '%s'", message.data.c_str());
  publisher->publish(message);

  // Wait until all matched subscribers ACK the message
  if (publisher->wait_for_all_acked(std::chrono::seconds(PUBLISHER_ACK_WAIT_SEC))) {
    RCLCPP_INFO(node->get_logger(), "Message delivered to subscribers");
  } else {
    RCLCPP_WARN(node->get_logger(), "Timeout waiting for ACKs");
  }
    
  rclcpp::sleep_for(PUBLISHER_KEEP_ALIVE_MSEC);

  rclcpp::shutdown();
  return 0;
}