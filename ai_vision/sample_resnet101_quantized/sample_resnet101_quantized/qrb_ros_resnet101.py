# Copyright (c) 2025 Qualcomm Innovation Center, Inc. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause-Clear


import os
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from std_msgs.msg import String
import cv2
import numpy as np
import subprocess
import sys
from ament_index_python.packages import get_package_share_directory
import shutil


class ResNet101QuantizedNode(Node):
    def __init__(self):
        super().__init__('resnet101_quantized_node')
        self.subscriber = self.create_subscription(
            Image,
            'image_raw',  
            self.image_callback,
            10
        )
        self.publisher = self.create_publisher(
            String,
            'resnet101_quantized_results', 
            10
        )
        self.bridge = CvBridge()
        self.get_logger().info(f'Initial ROS Node resnet101 quantized')
        
        self.declare_parameter('model_path', '/opt/model/')
        
        self.model_path = self.get_parameter('model_path').value
        
        self.get_logger().info(f'model path: {self.model_path}')
    
    def process(self):
        raw_file = open(workdir+"/output/Result_0/class_logits.raw", 'rb')
        raw_data = np.fromfile(raw_file,dtype=np.uint8)
        max_index = np.argmax(raw_data)
        with open(self.model_path+"/imagenet_labels.txt", 'r') as file:
            lines = file.readlines()
        
        return lines[max_index]

    def image_callback(self, msg):
        try:
            bridge = CvBridge()
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
            
            cv_image = cv2.resize(cv_image, (224, 224))
            
            cv_image.astype(np.uint8).tofile(workdir+"/input.raw")
        
            command = f"cd {workdir}; qtld-net-run --model {self.model_path}/ResNet101Quantized.tflite --input {workdir}/input --output {workdir}/output"
            subprocess.run(command, stdout=subprocess.DEVNULL, check=True, shell=True)
            result = self.process()
            self.publisher.publish(String(data=result))
            self.get_logger().info(f'publish resnet101 quantized {result} ')
        except Exception as e:
            self.get_logger().error(f'Error processing image: {e}')

def main(args=None):
    package_share_directory = get_package_share_directory('sample_resnet101_quantized')
    print(f"package_share_directory is {package_share_directory}")
    global workdir 
    workdir = package_share_directory
    if not os.path.exists(workdir + "/output"):
        os.makedirs(workdir+"/output" )
        os.chmod(workdir+"/output", 0o777)
         
    rclpy.init(args=args)
    node = ResNet101QuantizedNode() 
       
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Node stopped.')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
