#include "renderer_frontend.hh"
#include "renderer_backend.hh"
#include "vulkan/vulkan_backend.hh"
#include <memory>
// static VKBackend vkrenderer = {};

// static std::unique_ptr<RendererBackend> backend = nullptr;
static RendererBackend *backend;

// Initialize the renderer and create the preferred backend
bool 
Renderer::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height, RendererSettings settings) {
  renderer_backend_create(RENDERER_BACKEND_VULKAN, &backend);
  auto type = backend->type;
  if (!backend->Initialize(name)) {
    std::cout << "ERROR: Failed to initialize renderer backend" << std::endl;
    return false;
  }
  return true;
}

void 
Renderer::Shutdown() {
  // vkrenderer.OnDestroy();
  backend->Shutdown();
  delete backend;
}

void 
Renderer::OnResize(uint16_t width, uint16_t height) {
  // vkrenderer.WindowResize(width, height);
  if (backend) {
    backend->Resized(width, height);
  } else {
    printf("Renderer backend does not exist for this resize: [%i,%i]\n", width, height);
  }
}

bool
Renderer::CreateModel(Pegasus::GameObject& obj) {
  // Builder model_builder = {
  //   obj.vertices,
  //   obj.indices
  // };
  // vkrenderer.AddModel(model_builder);
  return true;
}

bool 
Renderer::DrawFrame(RenderPacket packet) {
  // if (vkrenderer.IsInitialized()) {
  //   vkrenderer.BeginFrame();
  //   vkrenderer.EndFrame(packet);
  // }
  if (backend->BeginFrame(packet.delta_time)) {
      bool result = backend->EndFrame(packet.delta_time);
      if (!result) {
          std::cout << "Renderer::EndFrame() returned unsuccessfully" << std::endl;
          return false;
      }
  }
  return true;
}