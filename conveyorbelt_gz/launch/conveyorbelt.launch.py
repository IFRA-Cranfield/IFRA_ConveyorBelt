#!/usr/bin/python3

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

# ======= CITE OUR WORK ======= #
# You can cite our work with the following statement:
# IFRA-Cranfield (2023) Gazebo Fortress / GZ Sim Conveyor Belt Plugin. URL: https://github.com/IFRA-Cranfield/IFRA_ConveyorBelt.

# objectpose.launch.py:
# Launch file for the IFRA_ConveyorBelt GZ Sim simulation in ROS 2:

# Import libraries:
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, IncludeLaunchDescription, RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.launch_description_sources import PythonLaunchDescriptionSource

# ========== **GENERATE LAUNCH DESCRIPTION** ========== #
def generate_launch_description():

    # ***** GZ Sim ***** #
    # DECLARE GZ Sim WORLD file:
    world_gz = os.path.join(
        get_package_share_directory('ros2srrc_gz'),
        'worlds',
        'ros2srrc_gz.sdf')
    # DECLARE Gazebo LAUNCH file:
    gzSIM = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [os.path.join(get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')]
        ),
        launch_arguments={
            'gz_args': f'-r -v 1 "{world_gz}"',
            'on_exit_shutdown': 'true'
        }.items(),
    )

    # SpawnEntity service bridge for world "ros2srrc_GzWorld":
    gzSERVICE_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_spawn_service_bridge',
        arguments=['/world/ros2srrc_GzWorld/create@ros_gz_interfaces/srv/SpawnEntity'],
        output='screen'
    )

    # ***** RETURN LAUNCH DESCRIPTION ***** #
    return LaunchDescription([
        gzSIM,
        gzSERVICE_bridge
    ])
