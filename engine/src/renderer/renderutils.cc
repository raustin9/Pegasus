#include "renderer.hh"
#include "renderer/vkcommon.hh"
#include <fstream>
#include <vulkan/vulkan_core.h>

VkShaderModule
Renderer::LoadShader(VKCommonParameters& vkparams, std::string filename) {
    size_t shaderSize;
    char* shaderCode = NULL;

    std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        shaderSize = is.tellg();
        is.seekg(0, std::ios::beg);
        // Copy file content into a buffer
        shaderCode = new char[shaderSize];
        is.read(shaderCode, shaderSize);
        is.close();
        assert(shaderSize > 0);
    }

    if (shaderCode) {
        // Create a new shader module that will be used for pipeline creation
        VkShaderModuleCreateInfo moduleCreateInfo = {};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = shaderSize;
        moduleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode);

        VkShaderModule shaderModule;
        VK_CHECK(
            vkCreateShaderModule(vkparams.Device.Device, &moduleCreateInfo, vkparams.Allocator, &shaderModule));

        delete[] shaderCode;
        return shaderModule;
    } else {
        std::cerr << "Error: could not open shader file " << filename << std::endl;
        return VK_NULL_HANDLE;
    }
}

uint32_t 
Renderer::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags props, VkPhysicalDeviceMemoryProperties deviceMemoryProperties) {
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & props) == props) {
                return i;
            }
        }
        typeBits >>= 1;
    }

    std::cerr << "Error: could not find suitable memory type" << std::endl;
    assert(0);
    return (0);
}

// For one time commands
VkCommandBuffer 
Renderer::BeginSingleTimeCommands(VKCommonParameters& params) {
//    VkCommandBufferAllocateInfo allocinfo = {};
//    allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocinfo.commandPool = 
    return VK_NULL_HANDLE;
}

// For 
void Renderer::EndSingleTimeCommands(VKCommonParameters& params, VkCommandBuffer commandBuffer) {
    return;
}
