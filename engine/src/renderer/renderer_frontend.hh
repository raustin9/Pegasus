#pragma once

/**
 * renderer_frontend.hh
 * 
 * This file is the interface that the application and other layers will use to interact 
 * with the renderer subsystem.
 * This is a backend-agnostic interface that can be used by any of the renderer backend types.
 * Eventually, this will support OpenGL and DirectX as well, so this is necessary so that we 
 * are able to have multiple backend types without other parts of the engine having to worry
 * about that.
*/

#include "stdafx.hh"
#include "defines.hh"
#include "render_types.hh"
// #include "vulkan/vulkan_backend.hh"
#include "game_types.hh"



class QAPI Renderer {
public:
  static bool Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height, RendererSettings settings);
  static void Shutdown();
  static bool CreateModel(Pegasus::GameObject& obj);

  static void OnResize(uint16_t width, uint16_t height);
  static bool DrawFrame(RenderPacket packet);
  static void SetView(qmath::Mat4<float> view);
};