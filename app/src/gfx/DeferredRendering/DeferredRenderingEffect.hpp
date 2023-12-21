#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "GfxEffect.hpp"

namespace DeferredRendering {

class Effect : public GfxEffect {
public:
    void bind() override;
    void update(GfxEffect::RenderCommonData& commonData) override;
    void draw(std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> commonData) override;

private:
    struct RenderItemData {
        glm::mat4 model;
    };

    struct PostProcessData {};

    void initGDataPass();
    void initPostProcessPass();
    void initInstances();
    void initHaloPass();

    std::shared_ptr<Engine::CrossPlatformShaderProgram> m_GDataShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>    m_GDataPass;

    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_InstanceRenderData;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_InstanceMaterialRenderData;

    std::shared_ptr<Engine::CrossPlatformDepthStencilTexture> m_GDepthBuffer;
    std::shared_ptr<Engine::CrossPlatformRenderTexture>       m_GColorBuffer;
    std::shared_ptr<Engine::CrossPlatformRenderTexture>       m_GNormalBuffer;
    std::shared_ptr<Engine::CrossPlatformFramebuffer>         m_GFramebuffer;

    std::shared_ptr<Engine::CrossPlatformShaderProgram>           m_PostProcessShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>              m_PostProcessPass;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_PostProcessData;

    std::shared_ptr<Engine::CrossPlatformShaderProgram>           m_HaloShader;
    std::shared_ptr<Engine::CrossPlatformRenderPass>              m_HaloPass;
    std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> m_HaloVolumeRenderData;
};

}  // namespace DeferredRendering