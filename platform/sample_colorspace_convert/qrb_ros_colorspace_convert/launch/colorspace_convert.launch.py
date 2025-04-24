# Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear

from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    conversion_type_arg = DeclareLaunchArgument(
        'conversion_type',
        default_value='nv12_to_rgb8',
        description='The type of conversion'
    )

    latency_fps_test_arg = DeclareLaunchArgument(
        'latency_fps_test',
        default_value='true',
        description='Enable or disable latency FPS test'
    )

    return LaunchDescription([
        conversion_type_arg,
        latency_fps_test_arg,

        ComposableNodeContainer(
            name='component_colorconvert_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='qrb_ros_colorspace_convert',
                    plugin='qrb_ros::colorspace_convert::ColorspaceConvertNode',
                    parameters=[{
                        'conversion_type': LaunchConfiguration('conversion_type'),
                        'latency_fps_test': LaunchConfiguration('latency_fps_test'),
                    }],
                    extra_arguments=[{'use_intra_process_comms': True, 'log_level': 'INFO'}],
                ),
            ],
            output='screen',
        )
    ])
