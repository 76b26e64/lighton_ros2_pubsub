#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <pigpiod_if2.h>

// 角度(0-180) → パルス幅(µs) に変換
// SG90の典型: 500us〜2500us を 0〜180度に割り当て（個体差あり）
static int angle_to_pulse_us(int angle)
{
  angle = std::clamp(angle, 0, 180);
  const int min_us = 500;  // 0°
  const int max_us = 2500; // 180°
  return min_us + (max_us - min_us) * angle / 180;
}

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber()
      : Node("minimal_subscriber")
  {
    auto topic_callback =
        [this](std_msgs::msg::String::UniquePtr msg) -> void
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
      const int gpio = 18; // 信号線をつなぐGPIO（例: GPIO18=物理12番ピン）
      const int sleep_time = 3;

      // pigpioデーモンに接続
      int pi = pigpio_start(nullptr, nullptr);
      if (pi < 0)
      {
        std::cerr << "pigpio_start failed. Is pigpiod running?\n";
        std::cerr << "Try: sudo systemctl start pigpiod\n";
        return;
      }

      // GPIOを出力に（必須ではないが明示）
      set_mode(pi, gpio, PI_OUTPUT);
      auto move_to = [&](int angle)
      {
        int pulse = angle_to_pulse_us(angle);
        // パルス幅をµsで指定（50Hz相当のサーボ制御）
        set_servo_pulsewidth(pi, gpio, pulse);
        // std::cout << "Angle " << angle << " deg -> " << pulse << " us\n";
      };

      move_to(90);
      std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
      move_to(60);
      std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
      move_to(90);
      std::this_thread::sleep_for(std::chrono::seconds(sleep_time));

      // 出力停止（0を指定するとサーボ信号OFF）
      set_servo_pulsewidth(pi, gpio, 0);
      pigpio_stop(pi);
    };

    subscription_ = this->create_subscription<std_msgs::msg::String>("topic", 10, topic_callback);
  }

private:
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}