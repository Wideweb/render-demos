#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "GfxEffect.hpp"

namespace MotionBlur {

class Effect : public GfxEffect {
public:
    void bind() override;
    void update(GfxEffect::RenderCommonData& commonData) override;
    void draw(std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> commonData) override;

private:
    struct RenderItemData {
        glm::mat4 model;
    };

    struct MotionData {
        glm::mat4 prevView;
        glm::mat4 prevProj;
    };

    struct BlurData {
        glm::vec2 bufferSize;
    };

    void initMotionDataPass();
    void initTextureRenderPass();
    void initComputePass();
    void initInstances();

    std::shared_ptr<Engine::CrossPlatformShaderProgram>           m_SkyboxShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>              m_SkyboxRenderPass;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_SkyboxRenderData;

    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_BlurData;
    std::shared_ptr<Engine::CrossPlatformComputeProgram>          m_ComputeShader;
    std::shared_ptr<Engine::CrossPlatformComputePass>             m_ComputePass;
    std::shared_ptr<Engine::CrossPlatformRenderTexture>           m_BlurBuffer;

    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_InstanceComputeData;

    std::shared_ptr<Engine::CrossPlatformShaderProgram>           m_TextureShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>              m_TextureRenderPass;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_TextureRenderData;

    std::shared_ptr<Engine::CrossPlatformShaderProgram> m_MotionDataShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>    m_MotionDataPass;

    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_MotionRenderData;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_InstanceRenderData;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_InstanceMaterialRenderData;

    std::shared_ptr<Engine::CrossPlatformDepthStencilTexture> m_DepthBuffer;
    std::shared_ptr<Engine::CrossPlatformRenderTexture>       m_ColorBuffer;
    std::shared_ptr<Engine::CrossPlatformRenderTexture>       m_MotionBuffer;
    std::shared_ptr<Engine::CrossPlatformFramebuffer>         m_Framebuffer;

    glm::mat4 m_PrevView;
    glm::mat4 m_PrevProjection;

    glm::mat4 m_CurrView;
    glm::mat4 m_CurrProjection;
};

}  // namespace MotionBlur