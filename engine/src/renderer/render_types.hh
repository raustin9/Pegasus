#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
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