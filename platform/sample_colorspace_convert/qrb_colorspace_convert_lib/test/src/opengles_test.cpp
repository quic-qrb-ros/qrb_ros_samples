// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause-Clear

#include "qrb_colorspace_convert_lib/colorspace_convert.hpp"

// #include <BufferAllocator/BufferAllocator.h>
#include <fcntl.h>
#include <linux/dma-buf.h>
#include <linux/dma-heap.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>

#define ALIGN(x, y) (((x) + (y) - 1) & (~((y) - 1)))

static int alloc_dma_buf(int size)
{
  int heap_fd = open("/dev/dma_heap/system", O_RDONLY | O_CLOEXEC);
  if (heap_fd < 0) {
    std::cerr << "open dma heap failed" << std::endl;
    return -1;
  }

  struct dma_heap_allocation_data heap_data = {};
  heap_data.len = size;
  heap_data.fd_flags = O_RDWR | O_CLOEXEC;

  if (ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &heap_data) != 0) {
    std::cerr << "dma heap alloc failed, len: " << heap_data.len << std::endl;
    close(heap_fd);
    return -1;
  }
  return heap_data.fd;
}

static int mock_data_from_file(int size, const std::string & path)
{
  int fd = alloc_dma_buf(size);
  if (fd <= 0) {
    return -1;
  }
  char * dst = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (dst == MAP_FAILED) {
    std::cerr << "storage data: mmap failed" << std::endl;
    return -1;
  }

  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  if (ifs.is_open()) {
    ifs.read(dst, size);
  } else {
    std::cerr << "open file: " << path << "failed" << std::endl;
  }
  ifs.close();

  munmap(dst, size);
  return fd;
}

static void dump_data_to_file(int fd, int size, const std::string & path)
{
  char * dst = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  if (dst == MAP_FAILED) {
    std::cerr << "dump file mmap failed" << std::endl;
    return;
  }

  std::ofstream out(path, std::ios::out | std::ios::binary);
  if (out.is_open()) {
    out.write(dst, size);
  } else {
    std::cerr << "open file: " << path << "failed" << std::endl;
  }
  out.close();

  munmap(dst, size);
}

int test_nv12_to_rgb8()
{
  int width = 1280;
  int height = 720;

  int align_height = ALIGN(height, 1);
  int align_width = ALIGN(width, 64);

  int input_fd = mock_data_from_file(align_width * align_height * 4, "/data/src.yuv");
  int output_fd = alloc_dma_buf(align_width * align_height * 4);

  std::cout << "in fd: " << input_fd << ", out fd: " << output_fd << std::endl;

  if (input_fd < 0 || output_fd < 0) {
    std::cout << "buffer alloc failed" << std::endl;
    return 1;
  }

  int dup_fd = dup(output_fd);
  if (dup_fd < 0) {
    perror("dup fd failed");
    close(input_fd);
    close(output_fd);
    return -1;
  }

  qrb::colorspace_convert_lib::OpenGLESAccelerator accelerator;

  bool success = accelerator.nv12_to_rgb8(input_fd, dup_fd, align_width, align_height);
  if (!success) {
    std::cerr << "nv12 to rgb8 failed" << std::endl;
  } else {
    std::cout << "nv12 to rgb8 success" << std::endl;
    dump_data_to_file(output_fd, align_width * align_height * 4, "/data/dst.rgb8");
  }

  close(input_fd);
  close(dup_fd);

  return 0;
}

int test_rgb8_to_nv12()
{
  int width = 1280;
  int height = 720;

  int align_height = ALIGN(height, 1);
  int align_width = ALIGN(width, 256);

  int input_fd = mock_data_from_file(align_width * align_height * 3, "/data/src.rgb8");
  int output_fd = alloc_dma_buf(align_width * align_height * 3);

  if (input_fd < 0 || output_fd < 0) {
    std::cout << "buffer alloc failed" << std::endl;
    return 1;
  }

  int dup_fd = dup(output_fd);
  if (dup_fd < 0) {
    perror("dup fd failed");
    close(input_fd);
    close(output_fd);
    return -1;
  }
  qrb::colorspace_convert_lib::OpenGLESAccelerator accelerator;
  bool success = accelerator.rgb8_to_nv12(input_fd, dup_fd, align_width, align_height);
  if (!success) {
    std::cerr << "rgb8 to nv12 failed" << std::endl;
  } else {
    std::cout << "rgb8 to nv12 success" << std::endl;
    dump_data_to_file(output_fd, align_width * align_height * 3, "/data/dst.yuv");
  }

  close(input_fd);
  close(dup_fd);

  return 0;
}

int main()
{
  test_nv12_to_rgb8();
  test_rgb8_to_nv12();
  return 0;
}