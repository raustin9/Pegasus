#include "renderer_frontend.hh"
#include "renderer_backend.hh"
#include "vulkan/vulkan_backend.hh"
#include "containers/qvector.inl"
#include "core/qlogger.hh"
#include "core/qmemory.hh"
#include <memory>
// static VKBackend vkrenderer = {};

// static std::unique_ptr<RendererBackend> backend = nullptr;
static RendererBackend *backend;

// Initialize the renderer and create the preferred backend
bool 
Renderer::Initialize(std::string name, std::string asset_path, uint32_t width, uint32_t height, RendererSettings settings) {
  Vector<std::string> test;
  std::string teststr1 = "str1";
  std::string teststr2 = "str2"; 
  std::string teststr3 = "str3"; 
  std::string teststr4 = "str4"; 
  test.push(teststr1);
  test.push(teststr2);
  test.push(teststr3);
  test.push(teststr4);

  Vector<std::string> v2 = test;
  for (uint64_t i = 0; i < v2.size(); i++) {
      qlogger::Debug("%s ", v2[i].c_str());
  }  
  renderer_backend_create(RENDERER_BACKEND_VULKAN, &backend); 
  auto type = backend->type; 
  if (!backend->Initialize(name)) {    
    qlogger::Error("Failed to initialize renderer backend");
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
    qlogger::Info("Renderer backend does not exist for this resize: [%i,%i]", width, height);
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
          qlogger::Error("Renderer::EndFrame() returned unsuccessfully");
          return false;
      }
  }
  return true;
}