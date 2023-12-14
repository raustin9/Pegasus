#include "renderer/vulkan/vulkan_types.hh"
#include "renderer/render_types.hh"
#include "core/qlogger.hh"
#include "renderer/vulkan/vkshader_utils.hh"

// Builtin shaders that are not configurable
#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

bool
VKObjShader::Create(VKContext& context) {
    // Shader module init info
    char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT
    };

    for (uint32_t i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        if (!create_shader_module(
            context,
            BUILTIN_SHADER_NAME_OBJECT,
            stage_type_strs[i],
            stage_types[i],
            i,
            this->stages
        )) {
            qlogger::Error("Unable to create %s shader module for %s.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
            return false;
        }
    }

    // TODO: Descriptors

    return true;
}

void
VKObjShader::Destroy(VKContext& context) {

}

void
VKObjShader::Use(VKContext& context) {

}