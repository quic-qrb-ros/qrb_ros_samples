# Copyright (c) 2025 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear


import os  # For accessing environment variables
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.logging import get_logger
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    # Get USB_CAMERA_PATH from environment variables
    logger = get_logger('image_publisher_launch')
    package_name = 'sample_resnet101_quantized'  # Replace with your ROS package name
    package_path = get_package_share_directory(package_name)
    #image_path = os.path.join(package_path, 'cup.jpg')  # Construct the relative path
    #image_path = os.path.join(package_path, 'glasses.jpg')
    image_path = os.path.join(package_path, 'glasses.jpg')

    logger.info(f'IMAGE_PATH set to: {image_path}')
    
    return LaunchDescription([
        # Node for sample_resnet101_quantized
        DeclareLaunchArgument(
        'model_path',
        default_value='/opt/model/', 
        description='Path to the model file'
        ),
        Node(
            package='sample_resnet101_quantized',  # Replace with the actual package name
            executable='qrb_ros_resnet101',  # Replace with the actual executable name
            output='screen',  # Output logs to terminal
            parameters=[{'model_path': LaunchConfiguration('model_path')}]
        ),

        # Node for image_publisher
        Node(
            package='image_publisher',  # Package name
            executable='image_publisher_node',  # Executable name
            name='image_publisher_node',  # Node name
            output='screen',  # Output logs to terminal
            parameters=[
                {'filename': image_path},  # Use the constructed relative path
                {'rate': 10.0},  # Set the publishing rate to 10 Hz
            ],
        ),
    ])
