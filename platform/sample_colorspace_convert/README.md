# QRB Color Space Conversion ROS Node

## Overview

Qualcomm's smart devices, such as the RB3 Gen2, use NV12 as the default image color space conversion format. Generally, the more common color space format is RGB888. To embrace open source and facilitate developers in converting between these two formats, we have developed the color space conversion ROS node. The feature as follows:

- Provide ROS node include
  - API to convert nv12 to rgb8
  - API to convert rgb8 to nv12
- Support dmabuf fd as input / output
- Input / output image receive/send with QRB ROS transport
- Hardware accelerates with GPU by OpenGL ES

## Quick Start

> Note： This document 's build & run is the latest. If it conflict with the online document, please follow this.

We provide two ways to use this package.

<details>
<summary>Docker</summary>

#### Setup
Please follow this [steps](https://github.com/qualcomm-qrb-ros/qrb_ros_docker?tab=readme-ov-file#quickstart) to setup docker env.


#### Build

```shell
cd ~/qrb_ros_ws/src/qrb_ros_docker/scripts && \
bash docker_run.sh

git clone https://github.com/quic-qrb-ros/lib_mem_dmabuf.git
git clone https://github.com/quic-qrb-ros/qrb_ros_transport.git
git clone https://github.com/quic-qrb-ros/qrb_ros_color_space_convert.git
```

#### Run

```shell
export XDG_RUNTIME_DIR=/dev/socket/weston/
mkdir -p $XDG_RUNTIME_DIR
export WAYLAND_DISPLAY=wayland-1

ros2 launch qrb_ros_colorspace_convert colorspace_convert.launch.py 'conversion_type:=nv12_to_rgb8' 'latency_fps_test:=True'
```

</details>
 

<details>
<summary>QIRP-SDK</summary>

#### Setup
Please follow this [steps](https://quic-qrb-ros.github.io/main/getting_started/index.html) to setup qirp-sdk env.


#### Build

```shell
# prepare
mkdir -p <qirp_decompressed_workspace>/qirp-sdk/ros_ws
cd <qirp_decompressed_workspace>/qirp-sdk/ros_ws

git clone https://github.com/quic-qrb-ros/lib_mem_dmabuf.git
git clone https://github.com/quic-qrb-ros/qrb_ros_transport.git
git clone https://github.com/quic-qrb-ros/qrb_ros_color_space_convert.git


# build
colcon build --merge-install --packages-skip qrb_ros_transport_test --cmake-args \
  -DPYTHON_EXECUTABLE=${OECORE_NATIVE_SYSROOT}/usr/bin/python3 \
  -DPython3_NumPy_INCLUDE_DIR=${OECORE_NATIVE_SYSROOT}/usr/lib/python3.12/site-packages/numpy/core/include \
  -DPYTHON_SOABI=cpython-312-aarch64-linux-gnu \
  -DCMAKE_MAKE_PROGRAM=/usr/bin/make \
  -DBUILD_TESTING=OFF

```

#### Run
```shell
export XDG_RUNTIME_DIR=/dev/socket/weston/
mkdir -p $XDG_RUNTIME_DIR
export WAYLAND_DISPLAY=wayland-1

ros2 launch qrb_ros_colorspace_convert colorspace_convert.launch.py 'conversion_type:=nv12_to_rgb8' 'latency_fps_test:=True'
```


</details>

<br>


You can get more details from [here](https://quic-qrb-ros.github.io/main/index.html).

## Supported Types

| QRB ROS Transport Type          | ROS Interfaces          |
| ------------------------------- | ----------------------- |
| [qrb_ros::transport::type::Image](./qrb_ros_transport_image_type/include/qrb_ros_transport_image_type/image.hpp) | [sensor_msgs::msg::Image](https://github.com/ros2/common_interfaces/blob/rolling/sensor_msgs/msg/Image.msg) |

## Contributing

We would love to have you as a part of the QRB ROS community. Whether you are helping us fix bugs, proposing new features, improving our documentation, or spreading the word, please refer to our [contribution guidelines](https://github.qualcomm.com/jiaxshi/QRB-ROS-repository-template/blob/QRBROS/CONTRIBUTING.md) and [code of conduct](https://github.qualcomm.com/jiaxshi/QRB-ROS-repository-template/blob/QRBROS/CODE_OF_CONDUCT.md).

- Bug report: If you see an error message or encounter failures, please create a [bug report](https://github.qualcomm.com/jiaxshi/QRB-ROS-repository-template/issues)
- Feature Request: If you have an idea or if there is a capability that is missing and would make development easier and more robust, please submit a [feature request](https://github.qualcomm.com/jiaxshi/QRB-ROS-repository-template/issues)


## Authors

- **Vito Wang** - *Initial work* - [violet227 (Vito Wang)](https://github.com/violet227)

See also the list of [Contributors to qualcomm-qrb-ros/qrb_ros_color_space_convert](https://github.com/qualcomm-qrb-ros/qrb_ros_color_space_convert/graphs/contributors) who participated in this project.

## License

Project is licensed under the [BSD-3-clause License](https://spdx.org/licenses/BSD-3-Clause.html). See [LICENSE](https://github.qualcomm.com/jiaxshi/QRB-ROS-repository-template/blob/QRBROS/LICENSE) for the full license text.