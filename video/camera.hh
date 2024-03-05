/* Copyright 2013-2018 the Alfalfa authors
                       and the Massachusetts Institute of Technology

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

      1. Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#pragma once

#include "file_descriptor.hh"
#include "mmap.hh"
#include "summarize.hh"

#include <linux/videodev2.h>
#include <optional>

class CameraFD : public FileDescriptor
{
public:
  using FileDescriptor::FileDescriptor;

  void buffer_dequeued() { register_read(); }
};

class Camera : public Summarizable
{
private:
  static constexpr unsigned int NUM_BUFFERS = 16;

  uint16_t width_;
  uint16_t height_;
  uint32_t pixel_format_;
  std::string device_name_;

  CameraFD camera_fd_;
  std::vector<MMap_Region> kernel_v4l2_buffers_;
  unsigned int next_buffer_index = 0;

  void init();

  unsigned int frames_dequeued_ {};
  unsigned int successful_frames_dequeued_ {};
  unsigned int frames_skipped_ {};

public:
  Camera( const uint16_t width,
          const uint16_t height,
          const std::string& device_name,
          const uint32_t pixel_format,
          const unsigned int frame_rate );

  ~Camera();

  std::string_view borrow_next_frame();
  std::string_view borrow_most_recent_frame();
  void release_frame();

  FileDescriptor& fd() { return camera_fd_; }

  void summary( std::ostream& out ) const override;
  void reset_summary() override {};
};
