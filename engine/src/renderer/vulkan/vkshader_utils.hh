#pragma once
#include "defines.hh"
#include "containers/qvector.inl"
#include "vulkan_types.hh"

bool create_shader_module(
    VKContext& context,
    const char* name,
    const char* type_str,
    VkShaderStageFlagBits shader_stage_flag,
    uint32_t stage_index,
    VKShaderStage* shader_stages
    // Vector<VKShaderStage> &shader_stages
);