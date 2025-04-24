// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#ifndef QRB_COLORSPACE_CONVERT_LIB__OPENGLES_COMMON_HPP_
#define QRB_COLORSPACE_CONVERT_LIB__OPENGLES_COMMON_HPP_

#include <iostream>

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

#ifndef CHECK_GL_ERRORS
#define GL(func) func;
#else
#define GL(func)                                                                                   \
  func;                                                                                            \
  check_gl_error(__FILE__, __LINE__);
#endif

void check_gl_error(const char * file, int line);

namespace qrb::colorspace_convert_lib
{
/// A wrapper for OpenGL program
class GLProgram
{
public:
  GLProgram();
  ~GLProgram();

  /// set shaders for GL program
  /// @param vshader vertex shader string
  /// @param fshader fragment shader string
  bool set_shaders(const std::string & vshader, const std::string & fshader);

  /// GL program id
  inline GLuint id() const { return id_; }

private:
  GLuint id_ = GL_NONE;
  GLuint vs_ = GL_NONE;
  GLuint fs_ = GL_NONE;
};

}  // namespace qrb::colorspace_convert_lib

#endif  // QRB_COLORSPACE_CONVERT_LIB__OPENGLES_COMMON_HPP_
