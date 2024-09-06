#include "renderer_frontend.hh"
#include "renderer_backend.hh"
#include "vulkan/vulkan_backend.hh"
#include "containers/qvector.inl"
#include "core/qlogger.hh"
#include "core/qmemory.hh"
#include "qmath/qmath.hh"
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

    backend->near_clip = 0.01f;
    backend->far_clip  = 1000.0f;
    backend->projection = qmath::Mat4<float>::Perspective(
        qmath::deg_to_rad(45), 
        (float)800 / (float)600, 
        backend->near_clip, 
        backend->far_clip);
    // backend->view = qmath::Mat4<float>::GetTranslation(
    //     qmath::Vec3<float>::New(0.0f, 0.0f, 30.0f));
    // backend->view.Invert();

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
        backend->projection = qmath::Mat4<float>::Perspective(
            qmath::deg_to_rad(45), 
            static_cast<float>(width) / static_cast<float>(height), 
            backend->near_clip, 
            backend->far_clip
        );
        backend->Resized(width, height);
    } else {
        qlogger::Info("Renderer backend does not exist for this resize: [%i,%i]", width, height);
    }
}

bool
Renderer::CreateModel(Pegasus::GameObject& obj) {
    // Builder model_builder = {
    //     obj.vertices,
    //     obj.indices
    // };
    // vkrenderer.AddModel(model_builder);
    return true;
}

bool 
Renderer::DrawFrame(RenderPacket packet) {
    if (backend->BeginFrame(packet.delta_time)) {
        backend->UpdateGlobalState(
            backend->projection,
            backend->view,
            qmath::Vec3<float>::Zero(),
            qmath::Vec4<float>::One(),
            0 
        );

        static float angle = 0.01f;
        angle += 0.1f;
        qmath::Quaternion<float> rotation = qmath::Quaternion<float>::FromAxisAngle(
            qmath::Vec3<float>::Forward(),
            angle,
            false
        );
        auto model = rotation.ToRotationMatrix(
            qmath::Vec3<float>::Zero()
        );

        backend->UpdateObject(model);

        bool result = backend->EndFrame(packet.delta_time);
        if (!result) {
                qlogger::Error("Renderer::EndFrame() returned unsuccessfully");
                return false;
        }
    }
    return true;
}
  
// API Method to expose being able to set the view matrix from 
// anywhere in the application
void 
Renderer::SetView(qmath::Mat4<float> view) {
    backend->view = view;

    return;
}

void 
Renderer::CreateTexture(
    std::string& name,
    bool auto_release,
    int32_t width,
    int32_t height,
    int32_t channel_count,
    Vector<uint8_t>& pixels,
    texture& out_texture
) {
    backend->CreateTexture(
        name,
        auto_release,
        width,
        height,
        channel_count,
        pixels,
        out_texture 
    );
}

void 
Renderer::DestroyTexture(texture& texture) {
    backend->DestroyTexture(texture);
}