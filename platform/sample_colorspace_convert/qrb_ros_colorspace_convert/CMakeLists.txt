cmake_minimum_required(VERSION 3.8)
project(qrb_ros_colorspace_convert)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# add targets
ament_auto_add_library(${PROJECT_NAME} SHARED
  src/colorspace_convert.cpp
)

rclcpp_components_register_nodes(${PROJECT_NAME}
  "qrb_ros::colorspace_convert::ColorspaceConvertNode"
)

ament_auto_package(INSTALL_TO_SHARE launch config)
