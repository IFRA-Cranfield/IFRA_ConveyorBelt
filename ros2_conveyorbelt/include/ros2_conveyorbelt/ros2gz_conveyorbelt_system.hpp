/*

# ===================================== COPYRIGHT ===================================== #
#                                                                                       #
#  IFRA (Intelligent Flexible Robotics and Assembly) Group, CRANFIELD UNIVERSITY        #
#  Created on behalf of the IFRA Group at Cranfield University, United Kingdom          #
#  E-mail: IFRA@cranfield.ac.uk                                                         #
#                                                                                       #
#  Licensed under the Apache-2.0 License.                                               #
#  You may not use this file except in compliance with the License.                     #
#  You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0  #
#                                                                                       #
#  Unless required by applicable law or agreed to in writing, software distributed      #
#  under the License is distributed on an "as-is" basis, without warranties or          #
#  conditions of any kind, either express or implied. See the License for the specific  #
#  language governing permissions and limitations under the License.                    #
#                                                                                       #
#  IFRA Group - Cranfield University                                                    #
#  AUTHORS: Mikel Bueno Viso - Mikel.Bueno-Viso@cranfield.ac.uk                         #
#           Dr. Seemal Asif  - s.asif@cranfield.ac.uk                                   #
#           Prof. Phil Webb  - p.f.webb@cranfield.ac.uk                                 #
#                                                                                       #
#  Date: June, 2023.                                                                    #
#                                                                                       #
# ===================================== COPYRIGHT ===================================== #

# ===================================== COPYRIGHT ===================================== #
#                                                                                       #
#  Information and guidance on how to implement a Gazebo Fortress / GZ Sim              #
#  conveyor belt plugin has been taken from the usnistgov/ARIAC GitHub repo. In that    #
#  repository, the simulation of a conveyor belt is already implemented, and the source #
#  code can be found inside /ariac_plugins. This has been useful for the development    #
#  IFRA_ConveyorBelt plugin, which has been designed to comply with the IFRA            #
#  Gazebo Fortress / GZ Sim robot simulation.                                           #
#                                                                                       #
#  usnistgov/ARIAC repo in GitHub:                                                      #
#     Repository for ARIAC (Agile Robotics for Industrial Automation Competition),      #
#     consisting of kit building and assembly in a simulated warehouse.                 #
#                                                                                       #
#  Copyright (C) 2023, usnistgov/ARIAC                                                  #
#                                                                                       #
# ===================================== COPYRIGHT ===================================== #

# ======= CITE OUR WORK ======= #
# You can cite our work with the following statement:
# IFRA-Cranfield (2023) Gazebo Fortress / GZ Sim Conveyor Belt Plugin. URL: https://github.com/IFRA-Cranfield/IFRA_ConveyorBelt.

*/

#ifndef ROS2GZ_CONVEYORBELT_SYSTEM_HPP
#define ROS2GZ_CONVEYORBELT_SYSTEM_HPP

#include <memory>
#include <mutex>
#include <chrono>
#include <limits>

#include <rclcpp/rclcpp.hpp>
#include <conveyorbelt_msgs/msg/conveyor_belt_state.hpp>
#include <conveyorbelt_msgs/srv/conveyor_belt_control.hpp>

// Ignition Gazebo (Fortress)
#include <ignition/gazebo/System.hh>
#include <ignition/gazebo/Model.hh>
#include <ignition/gazebo/Entity.hh>
#include <ignition/gazebo/EntityComponentManager.hh>
#include <ignition/gazebo/Types.hh>
#include <ignition/gazebo/components/Name.hh>
#include <ignition/gazebo/components/JointVelocityCmd.hh>
#include <ignition/gazebo/components/JointPosition.hh>
#include <ignition/gazebo/components/JointPositionReset.hh>
#include <ignition/gazebo/Util.hh>

// Logging macros (ignerr / ignwarn / ignmsg)
#include <ignition/common/Console.hh>

namespace gz_conveyor
{
class ROS2ConveyorBeltSystem
  : public ignition::gazebo::System,
    public ignition::gazebo::ISystemConfigure,
    public ignition::gazebo::ISystemPreUpdate
{
public:
  ROS2ConveyorBeltSystem() = default;
  ~ROS2ConveyorBeltSystem() override = default;

  // Gazebo entry points
  void Configure(const ignition::gazebo::Entity &entity,
                 const std::shared_ptr<const sdf::Element> &sdf,
                 ignition::gazebo::EntityComponentManager &ecm,
                 ignition::gazebo::EventManager &) override;

  void PreUpdate(const ignition::gazebo::UpdateInfo &info,
                 ignition::gazebo::EntityComponentManager &ecm) override;

private:
  // ROS 2
  rclcpp::Node::SharedPtr ros_node_;
  rclcpp::Publisher<conveyorbelt_msgs::msg::ConveyorBeltState>::SharedPtr status_pub_;
  rclcpp::Service<conveyorbelt_msgs::srv::ConveyorBeltControl>::SharedPtr power_srv_;
  std::mutex mtx_;  // protects power_ and belt_velocity_

  // Model / joint
  ignition::gazebo::Entity model_{ignition::gazebo::kNullEntity};
  ignition::gazebo::Entity joint_{ignition::gazebo::kNullEntity};
  std::string joint_name_{"belt_joint"};

  // Params
  double max_velocity_{1.0};     // m/s
  double power_{0.0};            // [0..100]
  double belt_velocity_{0.0};    // m/s (derived)
  double lower_limit_{0.0};      // meters (unused for wrap; kept for completeness)
  double upper_limit_{0.01};     // meters (wrap distance target)
  double publish_rate_{1000.0};  // Hz

  // Wrap control
  bool pending_reset_{false};

  // Distance integrator (for deterministic wrap even without JointPosition)
  double travel_accum_{0.0};  // meters since last wrap
  std::chrono::nanoseconds last_time_{std::chrono::nanoseconds(0)};

  // Publish throttling (by sim time)
  std::chrono::nanoseconds publish_period_{std::chrono::nanoseconds(1'000'000)};
  std::chrono::nanoseconds last_pub_sim_{std::chrono::nanoseconds(0)};

  // Helpers
  void onSetPower(const conveyorbelt_msgs::srv::ConveyorBeltControl::Request::SharedPtr req,
                  conveyorbelt_msgs::srv::ConveyorBeltControl::Response::SharedPtr res);

  void publishStatus();
};
} // namespace gz_conveyor

#endif // ROS2GZ_CONVEYORBELT_SYSTEM_HPP
