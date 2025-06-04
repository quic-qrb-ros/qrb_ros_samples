// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#include "qrb_ros_colorspace_convert/colorspace_convert.hpp"

#include <sys/mman.h>

#include <fstream>

using namespace qrb_ros::transport::image_utils;
constexpr char input_topic_name[] = "image_raw";
constexpr char output_topic_name[] = "image";
const int calculate_time = 5;

namespace qrb_ros::colorspace_convert
{
ColorspaceConvertNode::ColorspaceConvertNode(const rclcpp::NodeOptions & options)
  : rclcpp::Node("colorspace_convert_node", options)
  , frame_cnt_(0)
  , total_latency_(0)
  , convert_latency_(0)
  , test_flag_(false)
  , accelerator_()
{
  // set parameter
  this->declare_parameter<std::string>("conversion_type", "nv12_to_rgb8");
  conversion_type_ = this->get_parameter("conversion_type").as_string();
  latency_fps_test_ = this->declare_parameter("latency_fps_test", false);

  rclcpp::SubscriptionOptions sub_options;
  rclcpp::PublisherOptions pub_options;

  sub_options.use_intra_process_comm = rclcpp::IntraProcessSetting::Enable;
  pub_options.use_intra_process_comm = rclcpp::IntraProcessSetting::Enable;

  // create subscriber()
  if (conversion_type_ == "rgb8_to_nv12") {
    RCLCPP_DEBUG(this->get_logger(), "colorspace convert type: rgb8_to_nv12");
    handle_sub_ = this->create_subscription<qrb_ros::transport::type::Image>(input_topic_name, 10,
        std::bind(&ColorspaceConvertNode::rgb8_to_nv12_callback, this, std::placeholders::_1),
        sub_options);
  } else if (conversion_type_ == "nv12_to_rgb8") {
    RCLCPP_DEBUG(this->get_logger(), "colorspace convert type: nv12_to_rgb8");
    handle_sub_ = this->create_subscription<qrb_ros::transport::type::Image>(input_topic_name, 10,
        std::bind(&ColorspaceConvertNode::nv12_to_rgb8_callback, this, std::placeholders::_1),
        sub_options);
  } else {
    RCLCPP_ERROR(this->get_logger(), "Invalid conversion type: %s", conversion_type_.c_str());
    throw std::runtime_error("Invalid conversion type!");
  }

  // create publisher()
  handle_pub_ =
      this->create_publisher<qrb_ros::transport::type::Image>(output_topic_name, 10, pub_options);
}

bool ColorspaceConvertNode::convert_core(const qrb_ros::transport::type::Image & handler,
    const std::string & type)
{
  // For test: set up a timer to calculate FPS every 5 seconds
  if (latency_fps_test_ && !test_flag_) {
    test_flag_ = true;
    fps_timer_ = this->create_wall_timer(std::chrono::seconds(calculate_time),
        std::bind(&ColorspaceConvertNode::calculate_fps_and_latency, this));
  }
  if (latency_fps_test_)
    node_start_time_ = std::chrono::steady_clock::now();

  std::string encoding = handler.encoding;
  uint32_t alignd_width = align_width(handler.width);
  uint32_t alignd_height = align_height(handler.height);

  if (encoding != "nv12" && encoding != "rgb8") {
    RCLCPP_ERROR(this->get_logger(), "Unsupported image encoding: %s", encoding.c_str());
    return false;
  }

  // core part
  bool success = false;
  auto out_msg = std::make_unique<qrb_ros::transport::type::Image>();
  int img_size = 0;
  if (encoding == "nv12")
    img_size = get_image_align_size(handler.width, handler.height, "rgb8");
  else
    img_size = get_image_align_size(handler.width, handler.height, "nv12");

  auto dmabuf = lib_mem_dmabuf::DmaBuffer::alloc(img_size, "/dev/dma_heap/system");

  out_msg->dmabuf = std::move(dmabuf);
  out_msg->header = handler.header;
  out_msg->width = handler.width;
  out_msg->height = handler.height;
  auto input_fd = handler.dmabuf->fd();
  auto output_fd = out_msg->dmabuf->fd();

  if (latency_fps_test_)
    convert_start_time_ = std::chrono::steady_clock::now();

  if (type == "nv12_to_rgb8") {
    out_msg->encoding = "rgb8";
    success = accelerator_.nv12_to_rgb8(input_fd, output_fd, alignd_width, alignd_height);
  } else if (type == "rgb8_to_nv12") {
    out_msg->encoding = "nv12";
    success = accelerator_.rgb8_to_nv12(input_fd, output_fd, alignd_width, alignd_height);
  }

  if (latency_fps_test_)
    convert_end_time_ = std::chrono::steady_clock::now();

  if (!success) {
    RCLCPP_ERROR(this->get_logger(), "colorspace convert failed!");
    return false;
  }

  // publish image
  handle_pub_->publish(std::move(out_msg));

  if (latency_fps_test_) {
    node_end_time_ = std::chrono::steady_clock::now();
    auto convert_time = std::chrono::duration_cast<std::chrono::microseconds>(
        convert_end_time_ - convert_start_time_)
                            .count();
    auto node_time =
        std::chrono::duration_cast<std::chrono::microseconds>(node_end_time_ - node_start_time_)
            .count();
    convert_latency_ += convert_time;
    total_latency_ += node_time;
    frame_cnt_++;
  }
  return true;
}

void ColorspaceConvertNode::nv12_to_rgb8_callback(const qrb_ros::transport::type::Image & handler)
{
  bool ret = convert_core(handler, "nv12_to_rgb8");
  if (!ret)
    RCLCPP_ERROR(this->get_logger(), "nv12_to_rgb8 convert fail!");
  else
    RCLCPP_DEBUG(this->get_logger(), "Convert done: nv12_to_rgb8");
}

void ColorspaceConvertNode::rgb8_to_nv12_callback(const qrb_ros::transport::type::Image & handler)
{
  bool ret = convert_core(handler, "rgb8_to_nv12");
  if (!ret)
    RCLCPP_ERROR(this->get_logger(), "rgb8_to_nv12 convert fail!");
  else
    RCLCPP_DEBUG(this->get_logger(), "Convert done: rgb8_to_nv12");
}

void ColorspaceConvertNode::calculate_fps_and_latency()
{
  double fps = static_cast<double>(frame_cnt_) / static_cast<double>(calculate_time);
  auto convert_latency = (static_cast<double>(convert_latency_) / frame_cnt_) / 1000;
  auto total_latency = (static_cast<double>(total_latency_) / frame_cnt_) / 1000;

  RCLCPP_INFO(this->get_logger(), "FPS: %.4f, convert_latency: %.4f ms, total_latency: %.4f ms",
      fps, convert_latency, total_latency);

  // Reset counters and start time
  frame_cnt_ = 0;
  convert_latency_ = 0;
  total_latency_ = 0;
}
}  // namespace qrb_ros::colorspace_convert

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(qrb_ros::colorspace_convert::ColorspaceConvertNode)
