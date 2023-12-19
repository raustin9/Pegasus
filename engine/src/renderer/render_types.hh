#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "stdafx.hh"
#include <glm/glm.hpp>
#include <qmath/qmath.hh>
// Uniform Buffer Object
struct UBO {
    alignas (16) glm::mat4 projectionView{1.f};
    // glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
};

// Settings to give to the renderer to determine certain behavior
struct RendererSettings {
  bool enable_validation = false;
  bool enable_vsync = false;
};

// Structure for a render packet
// This is sent from the application to the renderer
// The renderer uses information in this as data to render
struct RenderPacket {
    UBO ubo;
    float time;
    float delta_time;
};

// Structure for a vertex in the model
struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    // TODO: add normals and uv's

    static std::vector<VkVertexInputBindingDescription> GetBindingDesc();
    static std::vector<VkVertexInputAttributeDescription> GetAttribDesc();
    bool operator==(const Vertex& other) const {
        return position == other.position
                && color == other.color;
    }
};

struct global_uniform_object {
    qmath::Mat4<float> projection;
    qmath::Mat4<float> view;
    // PADDING
    char padding[256 - (2 * sizeof(qmath::Mat4<float>))];
};

// Used to create data for the model
struct Builder {
    std::vector <Vertex> vertices{};
    std::vector <uint32_t> indices{};

    // void LoadModels(const std::string& filepath);
};

// Types of renderer backends that this engine will support
enum renderer_backend_type : uint32_t {
    RENDERER_BACKEND_VULKAN,
    // ALL BELOW ARE NOT SUPPORTED YET
    RENDERER_BACKEND_OPENGL,
    RENDERER_BACKEND_DIRECTX,
    RENDERER_BACKEND_METAL,
};

// Pure Abstract Class for a renderer backend type
// This is the interface that other parts of the application
// will use to interact with the renderer. This also allows
// us to create different implimentations for each backend 
// type that we want to support.
class RendererBackend {
    public:
        RendererBackend() {}
        virtual bool Initialize(std::string& name) { return false; }
        virtual void Shutdown() {}

        virtual void Resized(uint32_t width, uint32_t height) {}

        virtual bool BeginFrame(float delta_time) { return false; }
        virtual void UpdateGlobalState(qmath::Mat4<float> projection, qmath::Mat4<float> view, qmath::Vec3<float> view_position, qmath::Vec4<float> ambient_color, int32_t mode) {}
        virtual bool EndFrame(float delta_time) { return false; }

        // Mutators and Accessors
        inline void     SetFrameNumber(uint64_t fn) { m_frame_number = fn; }
        inline uint64_t GetFrameNumber()            { return m_frame_number; }
        
        renderer_backend_type type;
    private:
        uint64_t m_frame_number;
};