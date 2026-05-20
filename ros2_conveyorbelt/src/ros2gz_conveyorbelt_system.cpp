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

#include "ros2_conveyorbelt/ros2gz_conveyorbelt_system.hpp"

#include <cmath>
#include <ignition/plugin/Register.hh>  // IGNITION_ADD_PLUGIN

using namespace gz_conveyor;
using namespace ignition;
using namespace ignition::gazebo;

void ROS2ConveyorBeltSystem::Configure(const Entity &entity,
                                       const std::shared_ptr<const sdf::Element> &sdf,
                                       EntityComponentManager &ecm,
                                       EventManager &)
{
  this->model_ = entity;
  Model model(entity);
  if (!model.Valid(ecm))
  {
    ignerr << "[ROS2ConveyorBeltSystem] Invalid model entity.\n";
    return;
  }

  // Read SDF params
  if (sdf)
  {
    if (sdf->HasElement("max_velocity"))
      this->max_velocity_ = sdf->Get<double>("max_velocity");
    if (sdf->HasElement("publish_rate"))
      this->publish_rate_ = sdf->Get<double>("publish_rate");
    if (sdf->HasElement("joint_name"))
      this->joint_name_ = sdf->Get<std::string>("joint_name");

    // Fortress has no JointPositionLimits component; allow overrides here
    if (sdf->HasElement("lower_limit"))
      this->lower_limit_ = sdf->Get<double>("lower_limit");
    if (sdf->HasElement("upper_limit"))
      this->upper_limit_ = sdf->Get<double>("upper_limit");
  }
  if (this->publish_rate_ <= 0.0) this->publish_rate_ = 1000.0;

  this->publish_period_ = std::chrono::nanoseconds(
      static_cast<int64_t>((1.0 / this->publish_rate_) * 1e9));

  // Resolve joint
  this->joint_ = model.JointByName(ecm, this->joint_name_);
  if (this->joint_ == kNullEntity)
  {
    ignerr << "[ROS2ConveyorBeltSystem] Joint [" << this->joint_name_ << "] not found.\n";
    return;
  }

  // Ensure command component exists
  if (!ecm.Component<components::JointVelocityCmd>(this->joint_))
    ecm.CreateComponent(this->joint_, components::JointVelocityCmd({0.0}));

  // ROS 2 node + comms
  if (!rclcpp::ok())
    rclcpp::init(0, nullptr);

  auto modelName = ecm.Component<components::Name>(this->model_)->Data();
  this->ros_node_ = std::make_shared<rclcpp::Node>(
      std::string("ros2gz_conveyorbelt_") + modelName);

  this->status_pub_ =
      this->ros_node_->create_publisher<conveyorbelt_msgs::msg::ConveyorBeltState>(
          "CONVEYORSTATE", rclcpp::QoS(10));

  this->power_srv_ =
      this->ros_node_->create_service<conveyorbelt_msgs::srv::ConveyorBeltControl>(
          "CONVEYORPOWER",
          std::bind(&ROS2ConveyorBeltSystem::onSetPower, this,
                    std::placeholders::_1, std::placeholders::_2));

  ignwarn << "[ROS2ConveyorBeltSystem] Loaded: joint=" << this->joint_name_
          << " vmax=" << this->max_velocity_
          << " pub_rate=" << this->publish_rate_
          << " limits=[" << this->lower_limit_ << ", " << this->upper_limit_ << "]\n";
}

void ROS2ConveyorBeltSystem::PreUpdate(
    const ignition::gazebo::UpdateInfo &info,
    ignition::gazebo::EntityComponentManager &ecm)
{
  // Spin ROS callbacks
  if (this->ros_node_)
    rclcpp::spin_some(this->ros_node_);

  if (info.paused || this->joint_ == ignition::gazebo::kNullEntity)
    return;

  // --- Sim time + dt ---
  const auto sim_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(info.simTime);
  double dt = 0.0;
  if (this->last_time_.count() > 0)
    dt = (sim_ns - this->last_time_).count() * 1e-9;
  this->last_time_ = sim_ns;

  // --- Read current joint position (if available) ---
  double q = std::numeric_limits<double>::quiet_NaN();
  if (auto pos = ecm.Component<ignition::gazebo::components::JointPosition>(this->joint_))
  {
    if (!pos->Data().empty())
      q = pos->Data()[0];
  }

  // --- Decide if we need to wrap (arm a one-tick reset) ---
  const double eps = 1e-3;  // ~1 mm safety margin

  bool arm_by_q = false;
  if (std::isfinite(q))
    arm_by_q = (q >= (this->upper_limit_ - eps));

  if (!this->pending_reset_)
  {
    // Integrate distance traveled by the belt since last frame (always available)
    this->travel_accum_ += std::abs(this->belt_velocity_) * dt;

    const bool arm_by_integrator = (this->travel_accum_ >= (this->upper_limit_ - eps));

    if (arm_by_q || arm_by_integrator)
    {
      this->pending_reset_ = true;
      this->travel_accum_  = 0.0;   // restart distance tally after each wrap
    }
  }

  // --- Command joint velocity; pause for one tick if we're resetting ---
  {
    std::scoped_lock lk(this->mtx_);
    auto *velCmd =
        ecm.Component<ignition::gazebo::components::JointVelocityCmd>(this->joint_);
    if (!velCmd)
      velCmd = ecm.CreateComponent(
          this->joint_, ignition::gazebo::components::JointVelocityCmd({0.0}));

    if (this->pending_reset_)
      velCmd->Data() = {0.0};                  // pause so reset can apply cleanly
    else
      velCmd->Data() = {this->belt_velocity_}; // normal conveyor motion
  }

  // --- If armed, perform the reset this tick and disarm ---
  if (this->pending_reset_)
  {
    auto *reset =
        ecm.Component<ignition::gazebo::components::JointPositionReset>(this->joint_);
    if (!reset)
      reset = ecm.CreateComponent(
          this->joint_, ignition::gazebo::components::JointPositionReset({0.0}));
    else
      reset->Data() = {0.0};

    this->pending_reset_ = false;
  }

  // --- Throttled publishes (by sim time) ---
  if (sim_ns - this->last_pub_sim_ >= this->publish_period_)
  {
    this->publishStatus();
    this->last_pub_sim_ = sim_ns;
  }

  // // Optional debug:
  // static int k = 0;
  // if ((k++ % 200) == 0)
  //   ignmsg << "[belt_joint] q=" << (std::isfinite(q) ? q : -1)
  //          << " s_accum=" << this->travel_accum_
  //          << " v_cmd=" << this->belt_velocity_
  //          << " upper=" << this->upper_limit_
  //          << " resetting=" << std::boolalpha << this->pending_reset_ << std::noboolalpha
  //          << "\n";
}

void ROS2ConveyorBeltSystem::onSetPower(
  const conveyorbelt_msgs::srv::ConveyorBeltControl::Request::SharedPtr req,
  conveyorbelt_msgs::srv::ConveyorBeltControl::Response::SharedPtr res)
{
  std::scoped_lock lk(this->mtx_);
  res->success = false;

  if (req->power >= 0.0 && req->power <= 100.0)
  {
    this->power_ = req->power;
    this->belt_velocity_ = this->max_velocity_ * (this->power_ / 100.0);
    res->success = true;
  }
  else
  {
    RCLCPP_WARN(this->ros_node_->get_logger(),
                "Conveyor power must be in [0,100], got %.3f", req->power);
  }
}

void ROS2ConveyorBeltSystem::publishStatus()
{
  conveyorbelt_msgs::msg::ConveyorBeltState msg;
  {
    std::scoped_lock lk(this->mtx_);
    msg.power   = this->power_;
    msg.enabled = (this->power_ > 0.0);
  }
  this->status_pub_->publish(msg);
}

// Register as an Ignition Gazebo (Fortress) System plugin
IGNITION_ADD_PLUGIN(
  gz_conveyor::ROS2ConveyorBeltSystem,
  ignition::gazebo::System,
  ignition::gazebo::ISystemConfigure,
  ignition::gazebo::ISystemPreUpdate
)
IGNITION_ADD_PLUGIN_ALIAS(gz_conveyor::ROS2ConveyorBeltSystem, "ros2_conveyorbelt_system")
