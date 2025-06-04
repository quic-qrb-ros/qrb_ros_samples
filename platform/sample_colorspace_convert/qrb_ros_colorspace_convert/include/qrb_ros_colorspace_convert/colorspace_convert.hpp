// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#ifndef QRB_ROS_COLORSPACE_CONVERT__COLORSPACE_CONVERT_HPP_
#define QRB_ROS_COLORSPACE_CONVERT__COLORSPACE_CONVERT_HPP_

#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "qrb_colorspace_convert_lib/colorspace_convert.hpp"
#include "qrb_ros_transport_image_type/image.hpp"

namespace qrb_ros::colorspace_convert
{
class ColorspaceConvertNode : public rclcpp::Node
{
public:
  explicit ColorspaceConvertNode(const rclcpp::NodeOptions & options);

private:
  qrb::colorspace_convert_lib::OpenGLESAccelerator accelerator_;
  std::shared_ptr<rclcpp::Subscription<qrb_ros::transport::type::Image>> handle_sub_;
  std::shared_ptr<rclcpp::Publisher<qrb_ros::transport::type::Image>> handle_pub_;
  std::string conversion_type_;
  void rgb8_to_nv12_callback(const qrb_ros::transport::type::Image & handler);
  void nv12_to_rgb8_callback(const qrb_ros::transport::type::Image & handler);
  bool convert_core(const qrb_ros::transport::type::Image & handler, const std::string & type);

  // test latency and fps
  rclcpp::TimerBase::SharedPtr fps_timer_;
  uint32_t frame_cnt_;
  std::chrono::time_point<std::chrono::steady_clock> node_start_time_;
  std::chrono::time_point<std::chrono::steady_clock> node_end_time_;
  std::chrono::time_point<std::chrono::steady_clock> convert_start_time_;
  std::chrono::time_point<std::chrono::steady_clock> convert_end_time_;
  uint64_t total_latency_;
  uint64_t convert_latency_;
  bool latency_fps_test_;
  bool test_flag_;
  void calculate_fps_and_latency();
};
}  // namespace qrb_ros::colorspace_convert

#endif  // QRB_ROS_COLORSPACE_CONVERT__COLORSPACE_CONVERT_HPP_
