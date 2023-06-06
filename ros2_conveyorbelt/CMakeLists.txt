cmake_minimum_required(VERSION 3.5)
project(ros2_conveyorbelt)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

############## COMPILE the ROS2ConveyorBelt Gazebo Plugin ##############

# Find required packages:
find_package(gazebo_ros REQUIRED)
find_package(gazebo_dev REQUIRED)
find_package(gazebo_msgs REQUIRED)
find_package(rclcpp REQUIRED)

find_package(conveyorbelt_msgs REQUIRED)

# /include:
include_directories(include)

# ROS2 Conveyor Belt Plugin:

include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})

add_library(ros2_conveyorbelt_plugin SHARED
  src/ros2_conveyorbelt_plugin.cpp
)
target_link_libraries(ros2_conveyorbelt_plugin ${GAZEBO_LIBRARIES})
ament_target_dependencies(ros2_conveyorbelt_plugin
  "rclcpp"
  "gazebo_dev"
  "gazebo_msgs"
  "gazebo_ros"
  "conveyorbelt_msgs"
)
ament_export_libraries(ros2_conveyorbelt_plugin)
install(
  TARGETS
    ros2_conveyorbelt_plugin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# =========================================================== #
# REQUIRED TO EXECUTE .py scripts:
install(
  DIRECTORY 
    python
  DESTINATION 
    share/${PROJECT_NAME}
)
install(PROGRAMS
  python/SpawnObject.py
  DESTINATION lib/${PROJECT_NAME} 
)

ament_package()