<!--

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

-->

<div id="top"></div>

<br />

<div align="center">

  <h2 align="center">IFRA_ConveyorBelt - Gazebo Fortress / GZ Sim Plugin</h2>

  <p align="center">
    IFRA (Intelligent Flexible Robotics and Assembly) Group
    <br />
    Centre for Robotics and Assembly
    <br />
    Cranfield University
  </p>
</div>

<br />

## ABOUT

### Intelligent Flexible Robotics and Assembly Group

The IFRA (Intelligent Flexible Robotics and Assembly) Group is part of the Centre for Robotics and Assembly at Cranfield University.

IFRA Group pushes technical boundaries. At IFRA we provide high-tech automation & assembly solutions, and we support smart manufacturing with Smart Industry technologies and solutions. Flexible Manufacturing Systems (FMS) are a clear example. They can improve overall operations and throughput quality by adapting to real-time changes and situations, supporting and pushing the transition towards flexible, intelligent and responsive automation, which we continuously seek and support.

The IFRA Group undertakes innovative research to design, create and improve Intelligent, Responsive and Flexible automation & assembly solutions, and this series of GitHub repositories provide background information and resources of how these developments are supported.

__SOCIAL MEDIA__:

IFRA-Cranfield:
- YouTube: https://www.youtube.com/@IFRACranfield
- LinkedIn: https://www.linkedin.com/in/ifra-cranfield/

Centre for Robotics and Assembly:
- Instagram: https://www.instagram.com/cranfieldrobotics/
- Facebook: https://www.facebook.com/cranfieldunirobotics/
- YouTube: https://www.youtube.com/@CranfieldRobotics
- LinkedIn: https://www.linkedin.com/company/cranfieldrobotics/
- Website: https://www.cranfield.ac.uk/centres/centre-for-robotics-and-assembly


### IFRA_ConveyorBelt Repository

The IFRA_ConveyorBelt repository has been developed in order to simulate the behaviour of a conveyor belt in a Gazebo Fortress / GZ Sim environment. This feature has been achieved thanks to the design and implementation of a Gazebo Fortress / GZ Sim plugin, which activates the conveyor with a simple service call in ROS 2. The repository contains a Gazebo package as well, where the plugin can be easily tested and verified with a simple box.

__VIDEO: Simple Conveyor Belt in Gazebo Fortress / GZ Sim__

[![Alt text](https://img.youtube.com/vi/8Ciuf99ukMs/0.jpg)](https://www.youtube.com/watch?v=8Ciuf99ukMs)

<br />

## INSTALLATION

The IFRA_ConveyorBelt tool (ROS 2 Humble, Gazebo Fortress / GZ Sim) can be easily downloaded and installed by executing the following commands:

```sh
cd ~/dev_ws/src
git clone -b humble-gzfortress https://github.com/IFRA-Cranfield/IFRA_ConveyorBelt.git
cd ~/dev_ws
colcon build
```

<br />

## USAGE

The "ros2_conveyorbelt" Gazebo Fortress / GZ Sim plugin has to be defined inside the conveyor belt model (.sdf) and it is loaded when the conveyor belt is spawned to the GZ Sim world. The plugin generates a ROS 2 service named /CONVEYORPOWER, which must be called in order to manipulate the speed of the conveyor belt.

__MAIN REQUIREMENT to execute the plugin: <plugin> tag in MODEL__

The following tag must be added to the .sdf file of the conveyor belt model:

```sh
<plugin name="ros2_conveyorbelt_system" filename="libros2gz_conveyorbelt_system.so">
  <max_velocity> </max_velocity>
  <publish_rate> </publish_rate>
  <joint_name> </joint_name>
  <lower_limit> </lower_limit>
  <upper_limit> </upper_limit>
</plugin>
```

The max_velocity, publish_rate, joint_name and upper/lower limit parameters can be manually modified, but it is recommended to leave them with the pre-defined values in conveyorbelt_gz/sdf/conveyor.sdf for optimal performance.

__EXAMPLE: Simple cube in conveyor belt__

The following steps must be followed in order to execute and simulate a simple box on top of the conveyor belt:

1. Launch the conveyor belt GZ Sim world environment:

    ```sh
    ros2 launch conveyorbelt_gz conveyorbelt.launch.py
    ```

2. Spawn the conveyor belt:

    ```sh
    ros2 run ros2srrc_execution SpawnObject.py --package "conveyorbelt_gz" --sdf "conveyor.sdf" --name "conveyor" --x 0.0 --y 0.0 --z 0.0
    ```

3. Spawn the box on top of the Belt:

    ```sh
    ros2 run ros2srrc_execution SpawnObject.py --package "conveyorbelt_gz" --sdf "RedCube.sdf" --name "RedCube" --x 0.0 --y -0.5 --z 0.9
    ```

4. Activate the conveyor belt with the desired speed -> Value = (0,100]:

    ```sh
    ros2 service call /CONVEYORPOWER conveyorbelt_msgs/srv/ConveyorBeltControl "{power: 10}"
    ```


<br />

## License

<p>
  Intelligent Flexible Robotics and Assembly Group
  <br />
  Created on behalf of the IFRA Group at Cranfield University, United Kingdom
  <br />
  E-mail: IFRA@cranfield.ac.uk
  <br />
  <br />
  Licensed under the Apache-2.0 License.
  <br />
  You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
  <br />
  <br />
  <a href="https://www.cranfield.ac.uk/">Cranfield University</a>
  <br />
  School of Aerospace, Transport and Manufacturing (SATM)
  <br />
    <a href="https://www.cranfield.ac.uk/centres/centre-for-robotics-and-assembly">Centre for Robotics and Assembly</a>
  <br />
  College Road, Cranfield
  <br />
  MK43 0AL, Bedfordshire, UK
  <br />
</p>

<br />

## References

Some of the core concepts and source code used to develop the IFRA_ConveyorBelt repository has been implemented thanks to the information and content that is publicly released in the following GitHub repositories:

- [usnistgov/ARIAC](https://github.com/usnistgov/ARIAC): Reference of how a conveyor belt can be simulated with a ROS 2 plugin has been taken from the "ariac2023" branch of this repository, and adapted for the IFRA ROS 2 robot simulation in Gazebo Fortress / GZ Sim.

- [rokokoo/gconveyor-demo](https://github.com/rokokoo/conveyor_demo): The CAD file (mesh) of the conveyor belt used in our demo has been taken from this repository.

Copyright statements acknowledging both repositories have been added to the source code.

<br />

## Cite our work

<p>
  You can cite our work with the following statement:
  <br />
  IFRA-Cranfield (2023) Gazebo Fortress / GZ Sim Conveyor Belt Plugin. URL: https://github.com/IFRA-Cranfield/IFRA_ConveyorBelt.
</p>

<br />

## Contact

<p>
  Mikel Bueno Viso - Research Assistant in Intelligent Automation at Cranfield University
  <br />
  E-mail: Mikel.Bueno-Viso@cranfield.ac.uk
  <br />
  LinkedIn: https://www.linkedin.com/in/mikel-bueno-viso/
  <br />
  Profile: https://www.cranfield.ac.uk/people/mikel-bueno-viso-32884399
  <br />
  <br />
  Dr. Seemal Asif - Lecturer in Artificial Intelligence and Robotics at Cranfield University
  <br />
  E-mail: s.asif@cranfield.ac.uk
  <br />
  LinkedIn: https://www.linkedin.com/in/dr-seemal-asif-ceng-fhea-miet-9370515a/
  <br />
  Profile: https://www.cranfield.ac.uk/people/dr-seemal-asif-695915
  <br />
  <br />
  Professor Phil Webb - Professor of Aero-Structure Design and Assembly at Cranfield University
  <br />
  E-mail: p.f.webb@cranfield.ac.uk
  <br />
  LinkedIn: https://www.linkedin.com/in/phil-webb-64283223/
  <br />
  Profile: https://www.cranfield.ac.uk/people/professor-phil-webb-746415
  <br />
</p>
