#include "renderer_frontend.hh"

static Renderer renderer = {};

bool 
RendererFrontend::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height) {
  renderer.Initialize(name, asset_path, width, height);
  renderer.OnInit();

  return true;
}

void 
RendererFrontend::Shutdown() {
  renderer.OnDestroy();
}

void 
RendererFrontend::OnResize(uint16_t width, uint16_t height) {
  renderer.WindowResize(width, height);
}

bool 
RendererFrontend::DrawFrame() {
  if (renderer.IsInitialized()) {
    renderer.BeginFrame();
    renderer.EndFrame();
  }
  return true;
}