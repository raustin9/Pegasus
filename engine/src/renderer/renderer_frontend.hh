#pragma once

#include "stdafx.hh"
#include "vulkan/vulkan_backend.hh"

struct render_packet {

};

class Renderer {
public:
  static bool Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height);
  static void Shutdown();

  static void OnResize(uint16_t width, uint16_t height);
  static bool DrawFrame(render_packet packet);
};