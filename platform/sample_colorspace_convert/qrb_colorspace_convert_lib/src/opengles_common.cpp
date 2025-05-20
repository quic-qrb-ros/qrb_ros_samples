// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#include "qrb_colorspace_convert_lib/opengles_common.hpp"

// check GL function errors
void check_gl_error(const char * file, int line)
{
  GLenum error;
  std::string error_str;

  while ((error = glGetError()) != GL_NO_ERROR) {
    switch (error) {
      case GL_NO_ERROR:
        error_str = "GL_NO_ERROR";
        break;
      case GL_INVALID_ENUM:
        error_str = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error_str = "GL_INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error_str = "GL_INVALID_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        error_str = "GL_OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error_str = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      default:
        std::cerr << "GLError: 0x" << std::hex << error << std::endl;
        break;
    }

    std::cerr << "GLError: " << error_str << "(" << file << ":" << line << ")" << std::endl;
    return;
  }
}

namespace qrb::colorspace_convert_lib
{
GLProgram::GLProgram() {}
GLProgram::~GLProgram()
{
  if (id_ != GL_NONE) {
    GL(glDeleteProgram(id_));
  }
}

bool GLProgram::set_shaders(const std::string & vshader, const std::string & fshader)
{
  GLint success = GL_NONE;
  const GLsizei error_log_size = 512;
  GLchar error_log[error_log_size];

  if (id_ == GL_NONE && (id_ = glCreateProgram()) == GL_NONE) {
    std::cerr << "Failed to create GL program: " << std::hex << glGetError() << std::endl;
    return false;
  }

  if ((vs_ = glCreateShader(GL_VERTEX_SHADER)) == 0) {
    std::cerr << "Failed to create GL vertex shader: " << std::hex << glGetError() << std::endl;
    return false;
  }

  const GLchar * code = vshader.c_str();

  GL(glShaderSource(vs_, 1, &code, NULL))
  GL(glCompileShader(vs_));

  GL(glGetShaderiv(vs_, GL_COMPILE_STATUS, &success))
  if (!success) {
    GL(glGetShaderInfoLog(vs_, error_log_size, NULL, error_log));
    std::cerr << "Failed to compile GL vertex shader: " << error_log << std::endl;
    return false;
  }

  if ((fs_ = glCreateShader(GL_FRAGMENT_SHADER)) == 0) {
    std::cerr << "Failed to create GL fragment shader: " << std::hex << glGetError() << std::endl;
    return false;
  }

  code = fshader.c_str();

  GL(glShaderSource(fs_, 1, &code, NULL));
  GL(glCompileShader(fs_));
  GL(glGetShaderiv(fs_, GL_COMPILE_STATUS, &success))
  if (!success) {
    glGetShaderInfoLog(fs_, error_log_size, NULL, error_log);
    std::cout << "Failed to compile GL fragment shader:" << error_log << std::endl;
    return false;
  }

  GL(glAttachShader(id_, vs_));
  GL(glAttachShader(id_, fs_));

  GL(glLinkProgram(id_));

  GLint linked = GL_NONE;
  GL(glGetProgramiv(id_, GL_LINK_STATUS, &linked));
  if (!linked) {
    std::cerr << "Failed to link GL program: " << std::hex << glGetError() << std::endl;
    return false;
  }

  GL(glDetachShader(id_, vs_));
  GL(glDetachShader(id_, fs_));

  GL(glDeleteShader(vs_));
  GL(glDeleteShader(fs_));

  return true;
}

}  // namespace qrb::colorspace_convert_lib
