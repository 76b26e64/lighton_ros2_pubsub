#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "controller.hpp"
#include "rpi3_light_controller.hpp"
#include "lighton_ros2_pubsub/pubsub.hpp"

using namespace std;

namespace {
  static unique_ptr<controller_lib::Controller> controller;
}

class Subscriber : public rclcpp::Node
{
public:
  Subscriber() : Node("controller_subscriber")
  {
    auto topic_callback =
        [this](std_msgs::msg::String::UniquePtr msg) -> void
    {
      RCLCPP_INFO(this->get_logger(), "Subscribe: '%s'", msg->data.c_str());
      controller->on();
    };

    subscription_ = this->create_subscription<std_msgs::msg::String>(TOPIC_NAME, rclcpp::QoS(1).reliable(), topic_callback);
  }

private:
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
  controller = controller_lib::Rpi3LightController::create();
  if(!controller){
    cerr << "Error controller can't create." << endl;
    return -1;
  }
  
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Subscriber>());
  rclcpp::shutdown();
  return 0;
}
