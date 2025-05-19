# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear

import os  # For accessing environment variables
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.logging import get_logger
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # Get USB_CAMERA_PATH from environment variables
    logger = get_logger('usb_cam_launch')

    # Declare the launch arguments for image_path and model_path
    usb_camera_path = DeclareLaunchArgument(
        'camera_path',
        default_value=os.environ.get('USB_CAMERA_PATH','/dev/video2')  # Default to /dev/video0 if not set
        description='Path to the USB camera device'
    )
    
    model_path_arg = DeclareLaunchArgument(
        'model_path',
        default_value='/opt/model/',
        description='Path to the model file'
    )
    
    # Use LaunchConfiguration to get the values of the arguments
    usb_camera_path = LaunchConfiguration('camera_path')
    model_path = LaunchConfiguration('model_path')
    
    logger.info(f'USB_CAMERA_PATH set to: {usb_camera_path}')
    logger.info(f'MODEL_PATH set to: {model_path}')

    return LaunchDescription([
        model_path_arg,
        usb_camera_path,
        
        # Node for sample_hand_detection
        Node(
            package='sample_hand_detection',  # Replace with the actual package name
            executable='qrb_ros_hand_detector',  # Replace with the actual executable name
            output='screen',  # Output logs to terminal
            parameters=[
                {'model_path': model_path},  # Use the model_path argument
            ],
        ),

        # Node for usb_cam
        Node(
            package='usb_cam',  # Package name
            executable='usb_cam_node_exe',  # Executable name
            name='usb_cam_node',  # Node name (optional)
            output='screen',  # Output logs to terminal
            parameters=[
                {'video_device': usb_camera_path},  # Fetch USB_CAMERA_PATH from environment
                {'pixel_format': 'mjpeg2rgb'},
                {'image_width': 640},
                {'image_height': 480},
                {'framerate': 10.0},
                {'queue_size': 1000},
            ],
        ),
    ])
