#pragma once
#include "vkcommon.hh"

// Structure to hold the configuration for the pipeline
struct VKPipelineConfig {
    VKPipelineConfig(const VKPipelineConfig&) = delete;
    VKPipelineConfig& operator= (const VKPipelineConfig&) = delete;

    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class VKPipeline {
    public:
        VKPipeline(
            VKCommonParameters &vkparams,
            VKGraphicsParameters &graphics, 
            const VKPipelineConfig &config
        );
        ~VKPipeline();
        VKPipeline(const VKPipeline&) = delete;
        VKPipeline& operator= (const VKPipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Destroy();

        // TODO: default pipeline config
        void CreateGraphicsPipeline(
                const std::string& vertShaderPath,
                const std::string& fragShaderPath);


    private:
        VKCommonParameters &m_vkparams;
        VKGraphicsParameters &m_vkgraphics;
        const VKPipelineConfig& m_config; // configuration for the pipeline
};
