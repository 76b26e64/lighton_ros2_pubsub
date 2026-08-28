cmake_minimum_required(VERSION 3.8)
project(lighton_ros2_pubsub)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

find_library(CONTROLLER_LIB controller_lib PATHS /usr/local/lib REQUIRED)

add_executable(subscriber src/subscriber.cpp)
target_include_directories(subscriber PRIVATE include)
ament_target_dependencies(subscriber rclcpp std_msgs)
target_link_libraries(subscriber controller_lib pigpiod_if2)

add_executable(publisher src/publisher.cpp)
target_include_directories(publisher PRIVATE include)
ament_target_dependencies(publisher rclcpp std_msgs)

install(TARGETS
  publisher
  subscriber
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
