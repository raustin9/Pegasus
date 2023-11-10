#include "vkpipeline.hh"
#include "renderer.hh"
#include "renderer/vkcommon.hh"

VKPipeline::VKPipeline(
    VKCommonParameters &vkparams,
    VKGraphicsParameters  &graphics,
    const VKPipelineConfig& config
) : m_vkparams(vkparams),
    m_vkgraphics(graphics), 
    m_config(config)
{
    (void)m_config;
}

VKPipeline::~VKPipeline() {

};

// 
// PUBLIC
//

void
VKPipeline::Bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkgraphics.GraphicsPipeline);
}

void
VKPipeline::Destroy() {
    vkDestroyPipelineLayout(m_vkparams.Device.Device, m_vkgraphics.PipelineLayout, m_vkparams.Allocator);
    vkDestroyPipeline(m_vkparams.Device.Device, m_vkgraphics.GraphicsPipeline, m_vkparams.Allocator);
}

// Create the graphics pipeline
void
VKPipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath) {
    //
    // INPUT ASSEMBLER
    //
    // Vertex binding descriptions describe the input assembler binding points where vertex buffers
    // will bound. This uses a single vertex buffer at binding point 0
    VkVertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding = 0;
    vertexInputBinding.stride = sizeof(VKModel::Vertex);
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Vertex attribute descriptions describe the vertex shader attribute locations and memory layouts
    // as well as the binding points from which the input assembler should retrieve data to pass
    // to the corresponding vertex shader input attributes
    // These match the following shader layout
    // layout (location = 0) in vec3 inPos;
    // layout (location = 0) in vec4 inColor;
    // Attribute location 0: position from vertex buffer at binding point 0
    std::vector<VkVertexInputBindingDescription> bindingDescriptions = VKModel::Vertex::GetBindingDesc();
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = VKModel::Vertex::GetAttribDesc();


    // Vertex input state used for pipeline creation
    // Vulkan spec uses it to specify the input of the entire pipeline
    // but since the first stage is almost always the input assembler, 
    // we can consider it as part of the input assembler state
    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = bindingDescriptions.data();;
    vertexInputState.vertexAttributeDescriptionCount = 2;
    vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Input assembly state describes how primitives are assembled by the input assembler
    // This pipeline will assemble vertex data as triangle lists (though we only have one triangle)
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    //
    // Rasterization state
    //
    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0.0f;
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasSlopeFactor = 0.0f;

    // 
    // Per-fragment operations
    //
    // Color blend state describes how blend factors are calculated
    // We need a blend state per color attachment (event if blending is not used)
    // because the pipeline needs to know the components/channels of the pixels in the color
    // attachments that can be written to
    VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
    blendAttachmentState[0].colorWriteMask = 0xf;
    blendAttachmentState[0].blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = blendAttachmentState;

    // Depth and stencil state containing depth and stencil information (compare and write operations)
    // We are not making use of this yet, and we could just pass a nullptr, but we can also explicitly
    // define a state indicating that the depth and stencil tests are disabled
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = {};
    depthStencilState.back = {};

    // Enable dynamic states
    //
    // Most states are stored into the pipeline, but there are a few dynamic ones
    // that can be changed within a command buffer
    // To be able to change these states dynamically, we need to specify which ones  
    // in the pipeline object are dynamic.
    // At that point, we can set the actual states later on in the command buffer
    //
    // Set the viewport and scissor as dynamic states
    std::vector<VkDynamicState> dynamicStateEnables;
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    // Viewport state sets the number of viewports and scissor used in this pipeline
    // We still need to set this information statically in the pipeline object
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    //
    // Multi sampling state
    //
    // We do not use this yet, but we can disable this for now
    // and enable it later
    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask = nullptr;

    // 
    // Shaders
    //
    // this only uses vertex and fragment shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    // Vertex shader
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // Set pipeline stage for this shader
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    // Load binary SPIR-V shader module
    shaderStages[0].module = Renderer::LoadShader(m_vkparams, vertPath);
    // Main entry point for the shader
    shaderStages[0].pName = "main";
    assert(shaderStages[0].module != VK_NULL_HANDLE);

    // Fragment shader
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // Set pipeline stage for this shader
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    // Load binary SPIR-V shader module
    shaderStages[1].module = Renderer::LoadShader(m_vkparams, fragPath);
    // Main entry point for the shader
    shaderStages[1].pName = "main";
    assert(shaderStages[1].module != VK_NULL_HANDLE);
   
    // 
    // Create graphics pipeline
    //

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = m_vkgraphics.PipelineLayout;
    pipelineCreateInfo.renderPass = m_vkgraphics.RenderPass;

    // Set pipeline shader stage info
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Assign the pipeline states to the pipeline creation info structure
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;

    // Create a graphics pipeline using the specified states
    VK_CHECK(
        vkCreateGraphicsPipelines(m_vkparams.Device.Device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, m_vkparams.Allocator, &m_vkgraphics.GraphicsPipeline));

    // SPIR-V shader modules are no longer needed once the pipeline has been created
    vkDestroyShaderModule(m_vkparams.Device.Device, shaderStages[0].module, m_vkparams.Allocator); 
    vkDestroyShaderModule(m_vkparams.Device.Device, shaderStages[1].module, m_vkparams.Allocator); 
    
}
