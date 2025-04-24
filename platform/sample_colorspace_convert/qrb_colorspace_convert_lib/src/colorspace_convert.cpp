// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#include "qrb_colorspace_convert_lib/colorspace_convert.hpp"

#include <drm/drm_fourcc.h>

#include <iostream>

namespace qrb::colorspace_convert_lib
{
OpenGLESAccelerator::~OpenGLESAccelerator()
{
  if (initialized_) {
    egl_deinit();
  }
}

bool OpenGLESAccelerator::egl_init()
{
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (display_ == EGL_NO_DISPLAY) {
    std::cerr << "Failed to get default display" << std::endl;
    return false;
  }

  EGLint major = 0, minor = 0;
  if (!eglInitialize(display_, &major, &minor)) {
    std::cerr << "egl init failed" << std::endl;
    return false;
  }

  // Set the rendering API in current thread.
  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    std::cerr << "Failed to set rendering API: " << std::hex << eglGetError() << std::endl;
    return false;
  }

  const EGLint attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

  // Create EGL rendering context.
  auto context_ = eglCreateContext(display_, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, attribs);

  if (context_ == EGL_NO_CONTEXT) {
    std::cerr << "Failed to create EGL context: " << std::hex << eglGetError() << std::endl;
    return false;
  }

  /// connect the context to the surface
  if (!eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_)) {
    std::cerr << "Failed to create EGL context: " << std::hex << eglGetError() << std::endl;
    eglDestroyContext(display_, context_);
    context_ = EGL_NO_CONTEXT;
    return false;
  }

  initialized_ = true;
  return true;
}

bool OpenGLESAccelerator::egl_deinit()
{
  if (display_ == EGL_NO_DISPLAY) {
    return true;
  }

  if (context_ != EGL_NO_CONTEXT) {
    if (!eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
      std::cerr << "Fail release EGL context: " << std::hex << eglGetError() << std::endl;
      return false;
    }
    if (!eglDestroyContext(display_, context_)) {
      std::cerr << "Fail destroy EGL context: " << std::hex << eglGetError() << std::endl;
      return false;
    }
    context_ = EGL_NO_CONTEXT;
  }

  if (eglTerminate(display_) == EGL_FALSE) {
    std::cerr << "Fail to terminate EGL: " << std::hex << eglGetError() << std::endl;
    return false;
  }

  display_ = EGL_NO_DISPLAY;
  initialized_ = false;
  return true;
}

bool OpenGLESAccelerator::nv12_to_rgb8(int in_fd, int out_fd, int width, int height)
{
  if (!initialized_ && !egl_init()) {
    std::cerr << "EGL init failed" << std::endl;
    return false;
  }

  static const char * vertex_shader = R"(
    #version 320 es
    precision highp float;

    layout (location = 0) in vec2 v_position;
    layout (location = 1) in vec2 in_uv;

    out vec2 uv;

    void main()
    {
      gl_Position = vec4(v_position, 0.0, 1.0);
      uv = in_uv;
    }
    )";

  static const char * fragment_shader = R"(
    #version 320 es
    #extension GL_OES_EGL_image_external_essl3 : require

    precision highp float;
    uniform samplerExternalOES ext_tex;

    in vec2 uv;
    out vec4 color;

    void main()
    {
      color = texture(ext_tex, uv);
    }
    )";

  GLuint framebuffer;
  GLuint textures[2];
  GL(glGenFramebuffers(1, &framebuffer));
  GL(glGenTextures(2, textures));

  // set input texture
  // clang-format off
  EGLint in_attribs[32] = {
    EGL_WIDTH, width,
    EGL_HEIGHT, height,
    EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_NV12,
    EGL_DMA_BUF_PLANE0_FD_EXT, in_fd,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, width,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
    EGL_DMA_BUF_PLANE1_PITCH_EXT, width,
    EGL_DMA_BUF_PLANE1_OFFSET_EXT, width * height,
    EGL_NONE
  };
  // clang-format on
  auto src_img = eglCreateImageKHR(display_, context_, EGL_LINUX_DMA_BUF_EXT, NULL, in_attribs);
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]));
  GL(glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, src_img));

  // set output texture
  // clang-format off
  EGLint out_attribs[32] = {
    EGL_WIDTH, width,
    EGL_HEIGHT, height,
    EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_BGR888,  // for external RGB888
    EGL_DMA_BUF_PLANE0_FD_EXT, out_fd,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, width * 3,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
    EGL_NONE
  };
  // clang-format on
  auto out_img = eglCreateImageKHR(display_, context_, EGL_LINUX_DMA_BUF_EXT, NULL, out_attribs);
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[1]));
  GL(glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, out_img));

  // bind output texture to framebuffer color attachment
  GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
  GL(glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, textures[1], 0));

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "frame buffer is not complete" << std::endl;
    return false;
  }

  GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
  GL(glViewport(0, 0, width, height));

  GLfloat verts[] = { -1.0f, 3.0f, -1.0f, -1.0f, 3.0f, -1.0f };
  GLfloat frags[] = { 0.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };

  GLProgram gl_program;
  if (!gl_program.set_shaders(vertex_shader, fragment_shader)) {
    return false;
  }
  GL(glUseProgram(gl_program.id()));

  GL(glActiveTexture(GL_TEXTURE0));
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]));

  GL(glEnableVertexAttribArray(0));
  GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts));
  GL(glEnableVertexAttribArray(1));
  GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, frags));

  GL(glDrawArrays(GL_TRIANGLES, 0, 3));

  GL(glFinish());

  GL(glDisableVertexAttribArray(0));
  GL(glDisableVertexAttribArray(1));
  GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  GL(glDeleteFramebuffers(1, &framebuffer));
  GL(glDeleteTextures(2, textures));

  GL(eglDestroyImageKHR(display_, src_img));
  GL(eglDestroyImageKHR(display_, out_img));

  return true;
}

bool OpenGLESAccelerator::rgb8_to_nv12(int in_fd, int out_fd, int width, int height)
{
  if (!initialized_ && !egl_init()) {
    std::cerr << "EGL init failed" << std::endl;
    return false;
  }

  static const char * vertex_shader = R"(
    #version 320 es
    precision highp float;

    layout (location = 0) in vec2 v_position;
    layout (location = 1) in vec2 in_uv;

    out vec2 uv;

    void main()
    {
      gl_Position = vec4(v_position, 0.0, 1.0);
      uv = in_uv;
    }
    )";

  static const char * fragment_shader = R"(
    #version 320 es
    #extension GL_OES_EGL_image_external_essl3 : require
    #extension GL_EXT_YUV_target : require

    precision highp float;
    uniform samplerExternalOES ext_tex;

    in vec2 uv;
    layout(yuv) out vec4 color;

    void main()
    {
      vec4 source = texture(ext_tex, uv);
      color = vec4(rgb_2_yuv(source.rgb, itu_601), 1.0);
    }
    )";

  GLuint framebuffer;
  GLuint textures[2];
  GL(glGenFramebuffers(1, &framebuffer));
  GL(glGenTextures(2, textures));

  // set input texture
  // clang-format off
  EGLint in_attribs[32] = {
    EGL_WIDTH, width,
    EGL_HEIGHT, height,
    EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_BGR888,  // for external RGB888,
    EGL_DMA_BUF_PLANE0_FD_EXT, in_fd,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, width * 3,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
    EGL_NONE
  };
  // clang-format on
  auto src_img = eglCreateImageKHR(display_, context_, EGL_LINUX_DMA_BUF_EXT, NULL, in_attribs);
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]));
  GL(glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, src_img));

  // set output texture
  // clang-format off
  EGLint out_attribs[32] = {
    EGL_WIDTH, width,
    EGL_HEIGHT, height,
    EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_NV12,
    EGL_DMA_BUF_PLANE0_FD_EXT, out_fd,
    EGL_DMA_BUF_PLANE0_PITCH_EXT, width,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
    EGL_DMA_BUF_PLANE1_PITCH_EXT, width,
    EGL_DMA_BUF_PLANE1_OFFSET_EXT, width * height,
    EGL_NONE
  };
  // clang-format on
  auto out_img = eglCreateImageKHR(display_, context_, EGL_LINUX_DMA_BUF_EXT, NULL, out_attribs);
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[1]));
  GL(glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, out_img));

  // bind output texture to framebuffer color attachment
  GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
  GL(glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, textures[1], 0));

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "frame buffer is not complete" << std::endl;
    return false;
  }

  GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
  GL(glViewport(0, 0, width, height));

  GLfloat verts[] = { -1.0f, 3.0f, -1.0f, -1.0f, 3.0f, -1.0f };
  GLfloat frags[] = { 0.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };

  GLProgram gl_program;
  if (!gl_program.set_shaders(vertex_shader, fragment_shader)) {
    return false;
  }
  GL(glUseProgram(gl_program.id()));

  GL(glActiveTexture(GL_TEXTURE0));
  GL(glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]));

  GL(glEnableVertexAttribArray(0));
  GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts));
  GL(glEnableVertexAttribArray(1));
  GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, frags));

  GL(glDrawArrays(GL_TRIANGLES, 0, 3));

  GL(glFinish());

  GL(glDisableVertexAttribArray(0));
  GL(glDisableVertexAttribArray(1));
  GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  GL(glDeleteFramebuffers(1, &framebuffer));
  GL(glDeleteTextures(2, textures));

  GL(eglDestroyImageKHR(display_, src_img));
  GL(eglDestroyImageKHR(display_, out_img));

  return true;
}

}  // namespace qrb::colorspace_convert_lib