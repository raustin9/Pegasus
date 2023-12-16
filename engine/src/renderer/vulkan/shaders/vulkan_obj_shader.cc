#include "renderer/vulkan/vulkan_types.hh"
#include "renderer/render_types.hh"
#include "core/qlogger.hh"
#include "renderer/vulkan/vkshader_utils.hh"
#include "qmath/qmath.hh"

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

    // Pipeline Creation
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(context.framebuffer_height);
    viewport.width = static_cast<float>(context.framebuffer_width);
    viewport.height = -static_cast<float>(context.framebuffer_height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context.framebuffer_width;
    scissor.extent.height = context.framebuffer_height;

    // Attributes
    uint32_t offset = 0;
    constexpr int32_t attribute_count = 1;
    VkVertexInputAttributeDescription attribute_descriptions[attribute_count];
    // Position
    VkFormat formats[attribute_count] {
        VK_FORMAT_R32G32B32_SFLOAT,
    };
    uint64_t sizes[attribute_count] = {
        sizeof(qmath::Vec3<float>),
    };

    for (uint32_t i = 0; i < attribute_count; i++) {
        attribute_descriptions[i].binding = 0;
        attribute_descriptions[i].location = i;
        attribute_descriptions[i].format = formats[i];
        attribute_descriptions[i].offset = offset;
        offset += sizes[i];
    }

    // TODO: Descriptor Set Layouts

    // Stages
    // NOTE: Should match the order of this->stages
    VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT];
    QAllocator::Zero(stage_create_infos, sizeof(stage_create_infos));
    for (uint32_t i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        stage_create_infos[i].sType = this->stages[i].create_info.sType;
        stage_create_infos[i] = this->stages[i].shader_stage_create_info;
    }

    if (!this->pipeline.Create(
        context,
        context.main_renderpass,
        attribute_count,
        attribute_descriptions,
        0,
        nullptr,
        OBJECT_SHADER_STAGE_COUNT,
        stage_create_infos,
        viewport,
        scissor,
        false
    )) {
        qlogger::Error("Failed to load graphics pipeline for object shader");
        return false;
    }

    return true;
}

void
VKObjShader::Destroy(VKContext& context) {
    this->pipeline.Destroy(context);

    for (uint32_t i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        vkDestroyShaderModule(context.device.logical_device, this->stages[i].handle, context.allocator);
        this->stages[i].handle = nullptr;
    }
}

void
VKObjShader::Use(VKContext& context) {

}