# QRB ROS Samples

## Overview

This repository is a comprehensive collection of QRB ROS (Robot Operating System) example codes. It serves as a valuable resource for developers and enthusiasts looking to explore and implement QRB functionalities within the ROS framework. Each example is designed to demonstrate specific features and use cases, providing a practical guide to enhance your understanding and application of QRB in ROS environments.

## List of AI Samples

| Sample                                                       | UBUNTU RB3 Gen2 | QLI  RB3 Gen2 | UBUNTU  IQ-9075 | QLI IQ-9075 | UBUNTU IQ-8 Beta | QLI IQ-8 Beta | Description                                                  |
| ------------------------------------------------------------ | --------------- | ------------- | --------------- | ----------- | ---------------- | ------------- | ------------------------------------------------------------ |
| [Hand detection](ai_vision/sample_hand_detection/)           | N               | N             | Y               | Y           | N                | N             | The Hand detection is a machine learning pipeline that predicts bounding boxes and pose skeletons of hands in an image. For model information, please refer to [MediaPipe-Hand-Detection](https://huggingface.co/qualcomm/MediaPipe-Hand-Detection). |
| [sample_resnet101_quantized](ai_vision/sample_resnet101_quantized/) | Y               | Y             | Y               | Y           | Y                | Y             | The Image Classification is a machine learning model that can classify images from the Imagenet dataset. For model information, please refer to [ResNet101Quantized](https://huggingface.co/qualcomm/ResNet101Quantized). |
| [speech recognition](ai_audio/sample_speech_recognition/)    | Y               | N             | Y               | N           | Y                | N             | captures the audio input and publishes the ros topic with the speech recognition result, For model information, please refer to [Whisper-Tiny-En - Qualcomm AI Hub](https://aihub.qualcomm.com/iot/models/whisper_tiny_en?domain=Audio) |

## List of Robotics Samples

| Sample                                                       | UBUNTU RB3 Gen2 | QLI  RB3 Gen2 | UBUNTU  IQ-9075 | QLI IQ-9075 | UBUNTU IQ-8 Beta | QLI IQ-8 Beta | Description                                                  |
| ------------------------------------------------------------ | --------------- | ------------- | --------------- | ----------- | ---------------- | ------------- | ------------------------------------------------------------ |
| [simulation_sample_amr_simple_motion](robotics/simulation_sample_amr_simple_motion) | Y               | Y             | Y               | Y           | Y                | Y             | The `AMR simple motion sample` is a Python-based ROS node used to control the simple movements of QRB AMRs within the simulator. This sample allows you to control the movement of QRB AMRs via publishing the ROS messages to `/qrb_robot_base/cmd_vel` topic. |

## List of Platform Samples

| Sample                                                       | UBUNTU RB3 Gen2 | QLI  RB3 Gen2 | UBUNTU  IQ-9075 | QLI IQ-9075 | UBUNTU IQ-8 Beta | QLI IQ-8 Beta | Description                                                  |
| ------------------------------------------------------------ | --------------- | ------------- | --------------- | ----------- | ---------------- | ------------- | ------------------------------------------------------------ |
| [ocr_service](platform/ocr_service)                          | Y               | Y             | Y               | Y           | Y                | Y             | The `ocr-service` sample application enables a service that provides the Optical Character Recognition (OCR) function. |
| [sample_colorspace_convert](platform/sample_colorspace_convert) | Y               | Y             | Y               | Y           | Y                | Y             | The `qrb-ros-color-space-convert` sample application converts between NV12 and RGB888 formats. |

## System Requirements

- QIRP SDK . [qualcomm-linux/meta-qcom-robotics-sdk](https://github.com/qualcomm-linux/meta-qcom-robotics-sdk)
- ROS 2 Jazzy and later.



## Contributions

Thanks for your interest in contributing to qrb_ros_interfaces! Please read our [Contributions Page](CONTRIBUTING.md) for more information on contributing features or bug fixes. We look forward to your participation!

## License

qrb_ros_samples is licensed under the BSD 3-clause "New" or "Revised" License.

Check out the [LICENSE](LICENSE) for more details.
