#include "renderer_frontend.hh"

static VKBackend vkrenderer = {};

bool 
Renderer::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height, RendererSettings settings) {
  vkrenderer.Initialize(name, asset_path, width, height, settings);
  // vkrenderer.OnInit();

  return true;
}

void 
Renderer::Shutdown() {
  vkrenderer.OnDestroy();
}

void 
Renderer::OnResize(uint16_t width, uint16_t height) {
  vkrenderer.WindowResize(width, height);
}

bool
Renderer::CreateModel(Pegasus::GameObject& obj) {

  return true;
}

bool 
Renderer::DrawFrame(RenderPacket packet) {
  if (vkrenderer.IsInitialized()) {
    vkrenderer.BeginFrame();
    vkrenderer.EndFrame(packet);
  }
  return true;
}