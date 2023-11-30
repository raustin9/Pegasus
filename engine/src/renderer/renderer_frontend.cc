#include "renderer_frontend.hh"
#include "renderer_backend.hh"
#include "vulkan/vulkan_backend.hh"
#include <memory>
// static VKBackend vkrenderer = {};

// static std::unique_ptr<RendererBackend> backend = nullptr;
static RendererBackend *backend;

bool 
Renderer::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height, RendererSettings settings) {
  // renderer_backend_create(RENDERER_BACKEND_VULKAN, std::move(backend)); 
  // if (!backend->Initialize(name)) {
  //   std::cout << "ERROR: Failed to initialize renderer backend" << std::endl;
  //   return false;
  // }
  // backend = std::make_unique<VulkanBackend>();

  // backend = new VulkanBackend();
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
}

void 
Renderer::OnResize(uint16_t width, uint16_t height) {
  // vkrenderer.WindowResize(width, height);
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
  return true;
}