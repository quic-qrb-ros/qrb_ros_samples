cmake_minimum_required(VERSION 3.8)
project(qrb_colorspace_convert_lib VERSION 0.1.0)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# add targets
ament_auto_add_library(${PROJECT_NAME} SHARED
  src/opengles_common.cpp
  src/colorspace_convert.cpp
)

target_link_libraries(${PROJECT_NAME}
  EGL
  GLESv2
)

if(BUILD_TESTING)

  include_directories(test/include)

  macro(add_test_module module)
    add_executable(${module}_test
      test/src/${module}_test.cpp
    )
    install(TARGETS
      ${module}_test
      DESTINATION bin/
    )
  endmacro(add_test_module)

  add_test_module(opengles)
  target_link_libraries(opengles_test
    ${PROJECT_NAME}
  )

endif()

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

ament_auto_package()
