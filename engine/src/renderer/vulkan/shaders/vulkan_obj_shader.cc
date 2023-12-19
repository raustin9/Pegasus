#include "renderer/vulkan/vulkan_types.hh"
#include "renderer/render_types.hh"
#include "../vulkan_utils.hh"
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

    // Global Descriptors
    VkDescriptorSetLayoutBinding global_ubo_layout_binding;
    global_ubo_layout_binding.binding = 0;
    global_ubo_layout_binding.descriptorCount = 1;
    global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_ubo_layout_binding.pImmutableSamplers = nullptr;
    global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo global_layout_info {};
    global_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    global_layout_info.bindingCount = 1;
    global_layout_info.pBindings = &global_ubo_layout_binding;
    VK_CHECK(vkCreateDescriptorSetLayout(
        context.device.logical_device,
        &global_layout_info,
        context.allocator,
        &this->global_descriptor_set_layout
    ));

    VkDescriptorPoolSize global_pool_size;
    global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_pool_size.descriptorCount = context.swapchain.image_count;

    VkDescriptorPoolCreateInfo global_pool_info {};
    global_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    global_pool_info.poolSizeCount = 1;
    global_pool_info.pPoolSizes = &global_pool_size;
    global_pool_info.maxSets = context.swapchain.image_count;
    VK_CHECK(vkCreateDescriptorPool(
        context.device.logical_device,
        &global_pool_info,
        context.allocator,
        &this->global_descriptor_pool
    ));

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
    constexpr uint32_t desciptor_set_layout_count = 1;
    VkDescriptorSetLayout layouts[desciptor_set_layout_count] = {
        this->global_descriptor_set_layout
    };


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
        desciptor_set_layout_count,
        layouts,
        OBJECT_SHADER_STAGE_COUNT,
        stage_create_infos,
        viewport,
        scissor,
        false
    )) {
        qlogger::Error("Failed to load graphics pipeline for object shader");
        return false;
    }

    // Create the uniform buffer
    if (!this->global_uniform_buffer.Create(
        context,
        sizeof(global_uniform_object),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true
    )) {
        qlogger::Error("Failed to create uniform buffer object for object shader");
        return false;
    }

    VkDescriptorSetLayout global_layouts[3] = {
        this->global_descriptor_set_layout,
        this->global_descriptor_set_layout,
        this->global_descriptor_set_layout
    };

    VkDescriptorSetAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = this->global_descriptor_pool;
    alloc_info.descriptorSetCount = 3;
    alloc_info.pSetLayouts = global_layouts;
    VK_CHECK(vkAllocateDescriptorSets(
        context.device.logical_device,
        &alloc_info,
        this->global_descriptor_sets
    ));

    return true;
}

void
VKObjShader::Destroy(VKContext& context) {
    VkDevice device = context.device.logical_device;
    this->global_uniform_buffer.Destroy(context);
    this->pipeline.Destroy(context);

    vkDestroyDescriptorPool(device, this->global_descriptor_pool, context.allocator);

    vkDestroyDescriptorSetLayout(device, this->global_descriptor_set_layout, context.allocator);

    // Destroy shader modules
    for (uint32_t i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        vkDestroyShaderModule(context.device.logical_device, this->stages[i].handle, context.allocator);
        this->stages[i].handle = nullptr;
    }
}

void
VKObjShader::Use(VKContext& context) {
    uint32_t image_index = context.image_index;

    this->pipeline.Bind(context.graphics_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS);
}

void
VKObjShader::UpdateGlobalState(VKContext& context) {
    uint32_t image_index = context.image_index;
    VkCommandBuffer command_buffer = context.graphics_command_buffers[image_index].handle;
    VkDescriptorSet global_descriptor = this->global_descriptor_sets[image_index];

    // Bind the global descriptor set to be updated
    vkCmdBindDescriptorSets(
        command_buffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        this->pipeline.layout,
        0,
        1,
        &global_descriptor,
        0,
        nullptr
    );
    
    uint32_t range = sizeof(global_uniform_object);
    uint64_t offset = 0;

    this->global_uniform_buffer.LoadData(context, offset, range, 0, &this->global_ubo);

    VkDescriptorBufferInfo buffer_info {};
    buffer_info.buffer = this->global_uniform_buffer.handle;
    buffer_info.offset = offset;
    buffer_info.range = range;

    VkWriteDescriptorSet descriptor_write {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = this->global_descriptor_sets[image_index];
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(context.device.logical_device, 1, &descriptor_write, 0, nullptr);
    

}