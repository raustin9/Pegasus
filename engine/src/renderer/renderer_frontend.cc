#include "renderer_frontend.hh"

static VKBackend renderer = {};

bool 
Renderer::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height) {
  renderer.Initialize(name, asset_path, width, height);
  renderer.OnInit();

  return true;
}

void 
Renderer::Shutdown() {
  renderer.OnDestroy();
}

void 
Renderer::OnResize(uint16_t width, uint16_t height) {
  renderer.WindowResize(width, height);
}

bool 
Renderer::DrawFrame(render_packet packet) {
  if (renderer.IsInitialized()) {
    renderer.BeginFrame();
    renderer.EndFrame();
  }
  return true;
}