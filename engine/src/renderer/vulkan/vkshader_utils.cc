#include "vkshader_utils.hh"
#include "vulkan_utils.hh"
#include "core/qmemory.hh"
#include "core/qlogger.hh"
#include "platform/file_system.hh"

bool create_shader_module(
    VKContext& context,
    const char* name,
    const char* type_str,
    VkShaderStageFlagBits shader_stage_flag,
    uint32_t stage_index,
    // Vector<VKShaderStage> &shader_stages
    VKShaderStage* shader_stages
) {
    char file_name[512];
    sprintf(file_name, "assets/shaders/%s.%s.spv", name, type_str);

    QAllocator::Zero(&shader_stages[stage_index].create_info, sizeof(VkShaderModuleCreateInfo));
    shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    QFilesystem::QFile file;
    if (!file.open(file_name, QFilesystem::FILE_MODE_READ, true)) {
        qlogger::Error("Unable to read shader module: %s", file_name);
        return false;
    }

    // Read the .spv bytecode file as binary
    uint64_t size = 0;
    uint8_t* file_buffer = 0;
    if (!file.read_all_bytes(&file_buffer, size)) {
        qlogger::Error("Unable to read binary shader module: %s", file_name);
        return false;
    }

    shader_stages[stage_index].create_info.codeSize = size;
    // shader_stages[stage_index].create_info.pCode = reinterpret_cast<uint32_t*>(file_buffer);
    shader_stages[stage_index].create_info.pCode = (uint32_t*)(file_buffer);

    file.close();

    VK_CHECK(vkCreateShaderModule(
        context.device.logical_device,
        &shader_stages[stage_index].create_info,
        context.allocator,
        &shader_stages[stage_index].handle
    ));

    QAllocator::Zero(&shader_stages[stage_index].shader_stage_create_info, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[stage_index].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[stage_index].shader_stage_create_info.stage = shader_stage_flag;
    shader_stages[stage_index].shader_stage_create_info.module = shader_stages[stage_index].handle;
    shader_stages[stage_index].shader_stage_create_info.pName = "main";

    if (file_buffer) {
        QAllocator::Free(file_buffer, sizeof(uint8_t) * size, MEMORY_TAG_STRING);
        file_buffer = nullptr;
    }



    return true;
}