cmake_minimum_required(VERSION 3.10)
project(ros2_conveyorbelt)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(conveyorbelt_msgs REQUIRED)

# Fortress uses Ignition names:
find_package(ignition-gazebo6 REQUIRED)
find_package(ignition-math6 REQUIRED)         # optional, but common
find_package(ignition-transport11 REQUIRED)   # optional if you use gz transport

add_library(ros2gz_conveyorbelt_system SHARED
  src/ros2gz_conveyorbelt_system.cpp
)
target_include_directories(ros2gz_conveyorbelt_system PUBLIC include)
target_link_libraries(ros2gz_conveyorbelt_system
  ignition-gazebo6::ignition-gazebo6
  ignition-math6::ignition-math6
  ignition-transport11::ignition-transport11
)
ament_target_dependencies(ros2gz_conveyorbelt_system
  rclcpp
  conveyorbelt_msgs
)

install(TARGETS ros2gz_conveyorbelt_system LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

ament_package()