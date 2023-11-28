#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "stdafx.hh"
#include <glm/glm.hpp>
// Uniform Buffer Object
struct UBO {
    alignas (16) glm::mat4 projectionView{1.f};
    // glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
};

// Structure for a render packet
// This is sent from the application to the renderer
// The renderer uses information in this as data to render
struct RenderPacket {
    UBO ubo;
    float time;
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

// Used to create data for the model
struct Builder {
    std::vector <Vertex> vertices{};
    std::vector <uint32_t> indices{};

    // void LoadModels(const std::string& filepath);
};