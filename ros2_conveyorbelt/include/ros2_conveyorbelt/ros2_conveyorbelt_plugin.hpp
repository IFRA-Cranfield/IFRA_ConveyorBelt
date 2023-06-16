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
#  Information and guidance on how to implement a ROS2-Gazebo ConveyorBelt plugin has   #
#  been taken from the usnistgov/ARIAC repo in GitHub. In this repository, the          #
#  simulation of a ConveyorBelt is already being simulated, and the source code can     #
#  be found inside /ariac_plugins. This has been useful for the development of the      #
#  IFRA_ConveyorBelt plugin, which has been desinged in order to comply with the IFRA   # 
#  ROS2-Gazebo Robot Simulation.                                                        #
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
# IFRA-Cranfield (2023) Gazebo-ROS2 Conveyor Belt Plugin. URL: https://github.com/IFRA-Cranfield/IFRA_ConveyorBelt.

*/

#ifndef ROS2_CONVEYORBELT_PLUGIN_HPP_
#define ROS2_CONVEYORBELT_PLUGIN_HPP_

#include <gazebo/common/Plugin.hh>
#include <memory>

namespace gazebo_ros
{

class ROS2ConveyorBeltPluginPrivate;

class ROS2ConveyorBeltPlugin : public gazebo::ModelPlugin
{
public:
  /// Constructor:
  ROS2ConveyorBeltPlugin();

  /// Destructor:
  virtual ~ROS2ConveyorBeltPlugin();

  // LOAD plugin:
  void Load(gazebo::physics::ModelPtr _model, sdf::ElementPtr _sdf) override;

private:

  std::unique_ptr<ROS2ConveyorBeltPluginPrivate> impl_;
};

}  // namespace gazebo_ros

#endif  // ROS2_CONVEYORBELT_PLUGIN_HPP_