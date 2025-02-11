# Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import ComposableNodeContainer
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node
import os


def generate_launch_description():
    default_nms_iou_thres = "0.3"
    default_nms_score_thres = "0.4"

    label_file_arg = DeclareLaunchArgument(
        "label_file",
        description="label files for yolov8 model",
    )

    model_file_arg = DeclareLaunchArgument(
        "model",
        description="YOLOv8 detection model file path",
    )

    score_thres_arg = DeclareLaunchArgument(
        "score_thres",
        default_value=default_nms_score_thres,
        description="score(confidence) threshold value, between 0.0 ~ 1.0",
    )

    iou_thres_arg = DeclareLaunchArgument(
        "iou_thres",
        default_value=default_nms_iou_thres,
        description="iou threshold value, between 0.0 ~ 1.0",
    )

    # pub: /image camera node, 
    # pub: /camera_info camera info  
    camera_info_config_file_path = os.path.join(
        get_package_share_directory('qrb_ros_camera'),
        'config', 'camera_info_imx577.yaml'
    )
    camera_info_path = camera_info_config_file_path
    camera_node = ComposableNode(
        package='qrb_ros_camera',
        plugin='qrb_ros::camera::CameraNode',
        name='camera_node',
        parameters=[{
            'camera_info_path': camera_info_path,
            'fps': 30,
            'width': 1920,
            'height': 1080,
            'cameraId': 0,
            'publish_latency_type': 1,
        },
        {'reliability': 'best_effort'}
        ],
    )

    # ##colorspace_convert node
    # ## sub: /image_raw  
    # ## pub: /image  (remap to /image_convert)
    # convert_color_node = ComposableNode(
        # package="qrb_ros_colorspace_convert",
        # plugin="qrb_ros::colorspace_convert::ColorspaceConvertNode",
        # name="convert_color_node",
        # parameters=[{
                    # 'conversion_type': "nv12_to_rgb8", # or rgb8_to_nv12
                    # 'latency_fps_test': False, # False to disable test
                # },
                # {'reliability': 'best_effort'}
                # ],
        # extra_arguments=[{'use_intra_process_comms': True, 'log_level': 'INFO'}],
        # remappings=[
            # ("image", "image_convert"),
        # ],
    # )
    
    # ## resize_node
    # ## sub: image/image_raw
    # ## pub: resize/image_raw
    # resize_node = ComposableNode(
        # package="image_proc",
        # plugin="image_proc::ResizeNod",
        # name="resize_node",
        # parameters=[{
                    # 'use_scale': False,
                    # 'resize_height': 640,
                    # 'resize_width': 640,
                # },
                # {'reliability': 'best_effort'}
                # ],
    # )

    # convert_resize_node
    # sub: image
    # pub: image_convert
    # convert_resize_node = ComposableNode(
        # package="sample_object_detection",
        # plugin="sample_object_detection:",
        # name="image_convert_node",
        # parameters=[],
    # )
    
    ## sub: /yolo_input_img
    ## pub: /yolo_raw_img  (remap to /qrb_inference_input_tensor)
    preprocess_node = ComposableNode(
        package="qrb_ros_yolo_processor",
        plugin="qrb_ros::yolo_processor::YoloPreProcessNode",
        name="yolo_preprocess_node",
        parameters=[
        {'reliability': 'best_effort'}
        ],
        remappings=[
            ("yolo_input_img", "image_convert"),
        ],
    )

    ## sub: /qrb_inference_input_tensor
    ## pub: /qrb_inference_output_tensor (remap to /yolo_detect_tensor_output)
    inference_node = ComposableNode(
        package='qrb_ros_nn_inference',
        plugin='qrb_ros::nn_inference::QrbRosInferenceNode',
        name='nn_inference_node',
        parameters=[{
            'backend_option': "gpu",
            'model_path': LaunchConfiguration("model"),
        },
        {'reliability': 'best_effort'}
        ],
        remappings=[
             ("qrb_inference_input_tensor", "yolo_raw_img"),
        ]
    )

    ## sub: /yolo_detect_tensor_output
    ## pub: /yolo_detect_result
    postprocess_node = ComposableNode(
        package="qrb_ros_yolo_processor",
        plugin="qrb_ros::yolo_processor::YoloDetPostProcessNode",
        name="yolo_detection_postprocess_node",
        parameters=[
            {"label_file": LaunchConfiguration("label_file")},
            {"score_thres": LaunchConfiguration("score_thres")},
            {"iou_thres": LaunchConfiguration("iou_thres")},
            {'reliability': 'best_effort'}
        ],
         remappings=[
             ("yolo_detect_tensor_output", "qrb_inference_output_tensor"),
        ]
    )

    ## sub:
    ##   - /yolo_detect_result
    ##   - /yolo_input_img
    ## pub: /yolo_detect_overlay
    overlay_node = ComposableNode(
        package="qrb_ros_yolo_processor",
        plugin="qrb_ros::yolo_processor::YoloDetOverlayNode",
        name="yolo_detection_overlay_node",
        remappings=[
            ("yolo_input_img", "image_convert"),
        ],
        parameters=[
        {'reliability': 'best_effort'}
        ],
    )

    container = ComposableNodeContainer(
        name="sample_container",
        namespace="",
        package="rclcpp_components",
        executable="component_container",
        composable_node_descriptions=[camera_node,preprocess_node,inference_node,postprocess_node,overlay_node],
        #composable_node_descriptions=[camera_node],
        output="screen",
    )

    
    return LaunchDescription(
        [
            label_file_arg,
            model_file_arg,        
            score_thres_arg,
            iou_thres_arg,
            container,
            # Node(
                # package="sample_object_detection",
                # executable="image_converter_node",
                # name="image_converter_node",
                # parameters=[],
            # ),
        ]
    )
