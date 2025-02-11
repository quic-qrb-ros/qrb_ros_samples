# QRB ROS Samples

## Overview

qrb_ros_samples  is a ros2 package contains ai samples/robotics simulator samples. provides capabilities for Qualcomm Robotics Platforms to handle roboitics AI tasks. samples also serve to assemble Qualcomm Robotics End-to-End Scenarios

Model source : Qualcomm AI Hub / huggingface / github / tfhub

## List of samples

|           | AI audio         | Description                                                  | Model Source                                                 |
| --------- | ---------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| AI vision | Object detection | This is pipline of QRB ROS nodes to realize object detection, including camera/resize/convert/nn/yolo nodes | [YOLOv8-Detection - Qualcomm AI Hub](https://aihub.qualcomm.com/models/yolov8_det?searchTerm=yolo&domain=Computer+Vision) |

## System Requirements

- QIRP SDK . [qualcomm-linux/meta-qcom-robotics-sdk](https://github.com/qualcomm-linux/meta-qcom-robotics-sdk)
- ROS 2 Humble and later.

## Supported Platforms

This package is designed and tested to be compatible with ROS 2 Humble running on Qualcomm RB3 gen2.

| Hardware                        | Software                |
| ------------------------------- | ----------------------- |
| RB3 gen2                        | LE.QCROBOTICS.1.0       |

## Contributions

Thanks for your interest in contributing to qrb_ros_interfaces! Please read our [Contributions Page](CONTRIBUTING.md) for more information on contributing features or bug fixes. We look forward to your participation!

## License

qrb_ros_samples is licensed under the BSD 3-clause "New" or "Revised" License.

Check out the [LICENSE](LICENSE) for more details.
