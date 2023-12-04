#include "DeferredRenderingEffect.hpp"

#include "CascadeShadow.hpp"
#include "Frustrum.hpp"
#include "ModelFactory.hpp"

namespace DeferredRendering {

void Effect::bind() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    Engine::Mesh monkey = Engine::ModelLoader::loadObj("./../assets/models/monkey.obj");
    Engine::Mesh plane  = ModelFactory::createPlane(2.0f, 1, 1);
    Engine::Mesh box    = ModelFactory::createSphere(1.0f, 20, 20);

    render.registerGeometry("deferred-rendering", {"instance", "plane", "box"}, {monkey, plane, box});

    initGDataPass();
    initPostProcessPass();
    initInstances();
}

void Effect::initGDataPass() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    std::vector<Engine::ShaderProgramSlotDesc> slots = {
        {"cbCommon",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbObject",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbMaterial", Engine::SHADER_PROGRAM_SLOT_TYPE::DATA}
    };
    m_GDataShader = render.createShaderProgram(
        "./../assets/shaders/dx/deferred-data.hlsl", "./../assets/shaders/dx/deferred-data.hlsl", slots
    );

    uint32_t w, h;
    render.getViewport(w, h);

    m_GDepthBuffer  = render.createDepthStencilTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::D32, w, h);
    m_GColorBuffer  = render.createRenderTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::RGBA8, w, h);
    m_GNormalBuffer = render.createRenderTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::R32G32, w, h);

    m_GFramebuffer = render.createFramebuffer();
    m_GFramebuffer->addAttachment(m_GColorBuffer);
    m_GFramebuffer->addAttachment(m_GNormalBuffer);
    m_GFramebuffer->setDSAttachment(m_GDepthBuffer);

    Engine::CrossPlatformRenderPass::PipelineDesc pipelineDesc;
    pipelineDesc.cullMode        = Engine::CULL_MODE::BACK;
    pipelineDesc.depthClipEnable = true;
    pipelineDesc.depthFunc       = Engine::DEPTH_FUNC::LESS;

    std::vector<Engine::CROSS_PLATFROM_TEXTURE_FORMATS> rtvs = {
        Engine::CROSS_PLATFROM_TEXTURE_FORMATS::RGBA8, Engine::CROSS_PLATFROM_TEXTURE_FORMATS::R32G32
    };

    m_GDataPass =
        render.createRenderPass(m_GDataShader, rtvs, Engine::CROSS_PLATFROM_TEXTURE_FORMATS::D32, pipelineDesc);
}

void Effect::initPostProcessPass() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    std::vector<Engine::ShaderProgramSlotDesc> slots = {
        {"cbCommon",     Engine::SHADER_PROGRAM_SLOT_TYPE::DATA   },
        {"colorBuffer",  Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE},
        {"normalBuffer", Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE},
        {"depthBuffer",  Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE}
    };
    m_PostProcessShader = render.createShaderProgram(
        "./../assets/shaders/dx/deferred-postprocess.hlsl", "./../assets/shaders/dx/deferred-postprocess.hlsl", slots
    );

    Engine::CrossPlatformRenderPass::PipelineDesc pipelineDesc;
    pipelineDesc.cullMode        = Engine::CULL_MODE::NONE;
    pipelineDesc.depthClipEnable = false;
    pipelineDesc.depthFunc       = Engine::DEPTH_FUNC::LESS;

    m_PostProcessPass = render.createRenderPass(m_PostProcessShader, pipelineDesc);
}

void Effect::initInstances() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    m_InstanceRenderData         = render.createShaderProgramDataBuffer(sizeof(RenderItemData));
    m_InstanceMaterialRenderData = render.createShaderProgramDataBuffer(sizeof(GfxEffect::RenderMaterialData));

    RenderItemData itemData;
    itemData.model = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));
    itemData.model = glm::transpose(itemData.model);
    m_InstanceRenderData->copyData(&itemData);

    GfxEffect::RenderMaterialData instanceMaterial;
    instanceMaterial.diffuseAlbedo = glm::vec4(0.8f, 0.6f, 0.1f, 1.0f);
    instanceMaterial.fresnelR0     = glm::vec3(0.01f);
    instanceMaterial.roughness     = 0.5f;
    m_InstanceMaterialRenderData->copyData(&instanceMaterial);
}

void Effect::update(GfxEffect::RenderCommonData& commonData) {}

void Effect::draw(std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> commonData) {
    auto& app    = Engine::Application::get();
    auto& camera = app.getCamera();
    auto& render = app.getRender();
    auto& time   = app.getTime();

    uint32_t w, h;
    render.getViewport(w, h);

    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// PRE DRAW /////////////////////////////////
    m_GFramebuffer->resize(w, h);

    render.setFramebuffer(m_GFramebuffer);
    render.setPass(m_GDataPass);
    render.clear(0.0f, 0.0f, 0.0f, 0.0f);

    m_GDataShader->setDataSlot(0, commonData);
    m_GDataShader->setDataSlot(1, m_InstanceRenderData);
    m_GDataShader->setDataSlot(2, m_InstanceMaterialRenderData);

    render.drawItem("deferred-rendering", "instance");

    render.setFramebuffer(nullptr);
    ///////////////////////////////// PRE DRAW /////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// POSPROCESS ////////////////////////////////
    render.setPass(m_PostProcessPass);

    m_PostProcessShader->setDataSlot(0, commonData);
    m_PostProcessShader->setTextureSlot(1, m_GColorBuffer);
    m_PostProcessShader->setTextureSlot(2, m_GNormalBuffer);
    m_PostProcessShader->setTextureSlot(3, m_GDepthBuffer);

    render.drawItem("deferred-rendering", "plane");
}

}  // namespace DeferredRendering