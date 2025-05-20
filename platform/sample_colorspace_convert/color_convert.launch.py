# Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear

import launch
import os
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    camera_info_config_file_path = os.path.join(
        get_package_share_directory('qrb_ros_camera'),
        'config', 'camera_info_imx577.yaml'
    )
    camera_info_path = camera_info_config_file_path
    print(camera_info_path)
    """Generate launch description with multiple components."""
    container = ComposableNodeContainer(
    name='my_container',
    namespace='',
    package='rclcpp_components',
    executable='component_container',
    composable_node_descriptions=[
    ComposableNode(
        package='qrb_ros_camera',
        plugin='qrb_ros::camera::CameraNode',
        name='camera_node',
        parameters=[{
            'camera_info_path': camera_info_path,
            'fps': 30,
            'width': 640,
            'height': 480,
            'cameraId': 0,
        }],
        remappings=[    
            ('/image', '/image_raw'),
        ],
        ),
    ComposableNode(
        package='qrb_ros_colorspace_convert',
        plugin='qrb_ros::colorspace_convert::ColorspaceConvertNode',
        name='colorconvert',
        parameters=[{
            'conversion_type': 'nv12_to_rgb8',
        }],
        extra_arguments=[{'use_intra_process_comms': True}],
        ),
    ComposableNode(
        package='qrb_ros_transport_test',
        plugin='qrb_ros::transport::TestSubComponent',
        name='test',
        parameters=[{
            'test_type': 'qrb_ros::transport::type::Image',
            'topic_name': '/image',
            'dump_file': '/data/dump',
            'dump': True,
            'dump_camera_info_': False,
        }],
        extra_arguments=[{'use_intra_process_comms': True}],
        )
    ],
    output='screen',
    )

    return launch.LaunchDescription([container])