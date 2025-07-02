# QRB ROS Samples

## Overview

This repository is a comprehensive collection of QRB ROS (Robot Operating System) example codes. It serves as a valuable resource for developers and enthusiasts looking to explore and implement QRB functionalities within the ROS framework. Each example is designed to demonstrate specific features and use cases, providing a practical guide to enhance your understanding and application of QRB in ROS environments.



The `main` branch serves as the development branch and includes all samples currently under active development. It is primarily supported on Ubuntu by default.    For stable releases, please refer to the `jazzy-rel` branch.

## List of AI Samples

| Sample                                                       | Peripherals required | RB3 Gen2 Vision Kit | IQ-9075 Evaluation Kit | IQ-8 Beta   Evaluation Kit | Description                                                  |
| ------------------------------------------------------------ | -------------------- | ------------------- | ---------------------- | -------------------------- | ------------------------------------------------------------ |
| [Hand detection](ai_vision/sample_hand_detection/)           | N                    | N                   | Y                      | N                          | The Hand detection is a machine learning pipeline that predicts bounding boxes and pose skeletons of hands in an image. For model information, please refer to [MediaPipe-Hand-Detection](https://huggingface.co/qualcomm/MediaPipe-Hand-Detection). |
| [sample_resnet101_quantized](ai_vision/sample_resnet101_quantized/) | N                    | Y                   | Y                      | Y                          | The Image Classification is a machine learning model that can classify images from the Imagenet dataset. For model information, please refer to [ResNet101Quantized](https://huggingface.co/qualcomm/ResNet101Quantized). |
| [speech recognition](ai_audio/sample_speech_recognition/)    | N                    | Y                   | Y                      | Y                          | captures the audio input and publishes the ros topic with the speech recognition result, For model information, please refer to [Whisper-Tiny-En - Qualcomm AI Hub](https://aihub.qualcomm.com/iot/models/whisper_tiny_en?domain=Audio) |

## List of Robotics Samples

| Sample                                                       | Peripherals required | RB3 Gen2 Vision Kit | IQ-9075 Evaluation Kit | IQ-8 Beta   Evaluation Kit | Description                                                  |
| ------------------------------------------------------------ | -------------------- | ------------------- | ---------------------- | -------------------------- | ------------------------------------------------------------ |
| [simulation_sample_amr_simple_motion](robotics/simulation_sample_amr_simple_motion) | N                    | Y                   | Y                      | Y                          | The `AMR simple motion sample` is a Python-based ROS node used to control the simple movements of QRB AMRs within the simulator. This sample allows you to control the movement of QRB AMRs via publishing the ROS messages to `/qrb_robot_base/cmd_vel` topic. |
| 2D LiDAR SLAM                                                | RPLIDAR A3M1         | Y                   | N                      | N                          | The 2D LiDAR SLAM sample is designed based on `Cartographer`, which is capable of completing indoor map construction and localization based on 2D LiDAR sensors. |
| follow-me                                                    | Gemini 335L          | Y                   | N                      | N                          | The FollowMe is a lightweight application that enables robots to track targets in real-time. |

## List of Platform Samples

| Sample                                                       | Peripherals required | RB3 Gen2 Vision Kit | IQ-9075 Evaluation Kit | IQ-8 Beta   Evaluation Kit | Description                                                  |
| ------------------------------------------------------------ | -------------------- | ------------------- | ---------------------- | -------------------------- | ------------------------------------------------------------ |
| Orbbec-camera                                                | Gemini 335L          | Y                   | Y                      | N                          | The Orbbec-camera sample application enables the Orbbec Gemini camera 335L to work in RGB or depth mode. This application generates the RGB and depth information by topics. |
| RPLIDAR-ROS2                                                 | RPLIDAR A3M1         | Y                   | Y                      | Y                          | The RPLIDAR-ROS2 sample application enables the RPLIDAR A3M1 to work in RGB or depth mode. This application generates the RGB and depth information by topics. |
| [Qrb-ros-imu](https://github.com/qualcomm-qrb-ros/qrb_ros_imu) | N                    | Y                   | N                      | N                          | The QRB-ROS-IMU sample application enables the IMU to work in RGB or depth mode. This application generates the RGB and depth information by topics. |
| [Qrb-ros-system-monitor](https://github.com/qualcomm-qrb-ros/qrb_ros_system_monitor) | N                    | Y                   | Y                      | Y                          | The QRB-ROS-system-monitor sample application enables the system monitor to work in RGB or depth mode. This application generates the RGB and depth information by topics. |
| [Qrb-ros-battery]([qualcomm-qrb-ros/qrb_ros_battery](https://github.com/qualcomm-qrb-ros/qrb_ros_battery)) | N                    | Y                   | N                      | N                          | The QRB-ROS-battery sample application is a package that publishes the battery state data from the system node. |
| [Qrb-ros-camera](https://github.com/qualcomm-qrb-ros/qrb_ros_camera) | N                    | Y                   | N                      | N                          | The QRB-ROS-camera implements a camera ROS2 node to enable zero-copy performance when data is coming out of the camera-server. |
| [sample_ocr](platform/sample_ocr)                            | N                    | Y                   | Y                      | Y                          | The `ocr-service` sample application enables a service that provides the Optical Character Recognition (OCR) function. |
| [sample_colorspace_convert](platform/sample_colorspace_convert) | Y                    | Y                   | Y                      | Y                          | The `qrb-ros-color-space-convert` sample application converts between NV12 and RGB888 formats. |

## System Requirements

- LE.QCROBOTICS.1.0
- Canonical Ubuntu Image



## Contributions

Thanks for your interest in contributing to qrb_ros_interfaces! Please read our [Contributions Page](CONTRIBUTING.md) for more information on contributing features or bug fixes. We look forward to your participation!

## License

qrb_ros_samples is licensed under the BSD 3-clause "New" or "Revised" License.

Check out the [LICENSE](LICENSE) for more details.
