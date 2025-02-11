#!/usr/bin/python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import cv2
import numpy as np

class ImageConverterNode(Node):
    def __init__(self):
        super().__init__('image_convert_node')
        self.subscription = self.create_subscription(
            Image,
            'image',
            self.listener_callback,
            10)
        self.publisher_ = self.create_publisher(Image, 'image_convert', 10)
        self.bridge = CvBridge()
        self.target_width = 640
        self.target_height = 640

    def listener_callback(self, msg):
        try:
            nv12_image = np.frombuffer(msg.data, dtype=np.uint8).reshape((msg.height * 3 // 2, msg.width))
            
            yuv_image = cv2.cvtColor(nv12_image, cv2.COLOR_YUV2BGR_NV12)
            
            rgb_image = cv2.cvtColor(yuv_image, cv2.COLOR_BGR2RGB)
            
            resized_image = cv2.resize(rgb_image, (self.target_width, self.target_height))
            
            output_msg = self.bridge.cv2_to_imgmsg(resized_image, encoding='rgb8')
            
            self.publisher_.publish(output_msg)
        
        except CvBridgeError as e:
            self.get_logger().error('CvBridge Error: {0}'.format(e))

def main(args=None):
    rclpy.init(args=args)
    image_convert_node = ImageConverterNode()
    rclpy.spin(image_convert_node)
    image_convert_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
