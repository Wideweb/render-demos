#include "MotionBlurEffect.hpp"

#include "CascadeShadow.hpp"
#include "Frustrum.hpp"
#include "ModelFactory.hpp"

namespace MotionBlur {

void Effect::bind() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    Engine::Mesh monkey = Engine::ModelLoader::loadObj("./../assets/models/monkey.obj");
    Engine::Mesh plane  = ModelFactory::createPlane(2.0f, 1, 1);
    Engine::Mesh box    = ModelFactory::createSphere(1.0f, 20, 20);

    render.registerGeometry("motion-blur", {"instance", "plane", "box"}, {monkey, plane, box});

    initMotionDataPass();
    initComputePass();
    initTextureRenderPass();
    initInstances();
}

void Effect::initMotionDataPass() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    std::vector<Engine::ShaderProgramSlotDesc> slots = {
        {"cbCommon",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbObject",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbMaterial", Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbMotion",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA}
    };
    m_MotionDataShader = render.createShaderProgram(
        "./../assets/shaders/dx/motion-data.hlsl", "./../assets/shaders/dx/motion-data.hlsl", slots
    );

    uint32_t w, h;
    render.getViewport(w, h);

    m_DepthBuffer  = render.createDepthStencilTexture(w, h);
    m_ColorBuffer  = render.createRenderTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::RGBA8, w, h);
    m_MotionBuffer = render.createRenderTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::R32G32, w, h);

    m_Framebuffer = render.createFramebuffer();
    m_Framebuffer->setDSAttachment(m_DepthBuffer);
    m_Framebuffer->addAttachment(m_ColorBuffer);
    m_Framebuffer->addAttachment(m_MotionBuffer);

    Engine::CrossPlatformRenderPass::PipelineDesc pipelineDesc;
    pipelineDesc.cullMode        = Engine::CULL_MODE::BACK;
    pipelineDesc.depthClipEnable = true;
    pipelineDesc.depthFunc       = Engine::DEPTH_FUNC::LESS;

    std::vector<Engine::CROSS_PLATFROM_TEXTURE_FORMATS> rtvs = {
        Engine::CROSS_PLATFROM_TEXTURE_FORMATS::RGBA8, Engine::CROSS_PLATFROM_TEXTURE_FORMATS::R32G32
    };

    m_MotionDataPass =
        render.createRenderPass(m_MotionDataShader, rtvs, Engine::CROSS_PLATFROM_TEXTURE_FORMATS::D24S8, pipelineDesc);

    ///////////////////////////////// SKY BOX /////////////////////////////////

    std::vector<Engine::ShaderProgramSlotDesc> skyBoxPassSlots = {
        {"cbCommon", Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbObject", Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
        {"cbMotion", Engine::SHADER_PROGRAM_SLOT_TYPE::DATA},
    };
    m_SkyboxShader = render.createShaderProgram(
        "./../assets/shaders/dx/motion-skybox-data.hlsl", "./../assets/shaders/dx/motion-skybox-data.hlsl",
        skyBoxPassSlots
    );

    Engine::CrossPlatformRenderPass::PipelineDesc skyBoxPipelineDesc;
    skyBoxPipelineDesc.cullMode        = Engine::CULL_MODE::NONE;
    skyBoxPipelineDesc.depthClipEnable = true;
    skyBoxPipelineDesc.depthFunc       = Engine::DEPTH_FUNC::LESS_EQUAL;

    m_SkyboxRenderPass = render.createRenderPass(
        m_SkyboxShader, rtvs, Engine::CROSS_PLATFROM_TEXTURE_FORMATS::D24S8, skyBoxPipelineDesc
    );

    m_SkyboxRenderData = render.createShaderProgramDataBuffer(sizeof(RenderItemData));
}

void Effect::initComputePass() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    std::vector<Engine::ShaderProgramSlotDesc> slots = {
        {"cbData",       Engine::SHADER_PROGRAM_SLOT_TYPE::DATA              },
        {"colorBuffer",  Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE           },
        {"motionBuffer", Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE           },
        {"outputBuffer", Engine::SHADER_PROGRAM_SLOT_TYPE::READ_WRITE_TEXTURE}
    };
    m_ComputeShader = render.createComputeProgram("./../assets/shaders/dx/motion-blur.hlsl", slots);
    m_ComputePass   = render.createComputePass(m_ComputeShader);

    uint32_t w, h;
    render.getViewport(w, h);

    m_BlurBuffer = render.createRenderTexture(Engine::CROSS_PLATFROM_TEXTURE_FORMATS::RGBA8, w, h);
    m_BlurData   = render.createShaderProgramDataBuffer(sizeof(BlurData));
}

void Effect::initTextureRenderPass() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    std::vector<Engine::ShaderProgramSlotDesc> slots = {
        {"cbObject",   Engine::SHADER_PROGRAM_SLOT_TYPE::DATA   },
        {"diffuseMap", Engine::SHADER_PROGRAM_SLOT_TYPE::TEXTURE},
    };
    m_TextureShader = render.createShaderProgram(
        "./../assets/shaders/dx/screen-texture.hlsl", "./../assets/shaders/dx/screen-texture.hlsl", slots
    );

    Engine::CrossPlatformRenderPass::PipelineDesc pipelineDesc;
    pipelineDesc.cullMode        = Engine::CULL_MODE::NONE;
    pipelineDesc.depthClipEnable = false;
    pipelineDesc.depthFunc       = Engine::DEPTH_FUNC::LESS;

    m_TextureRenderPass = render.createRenderPass(m_TextureShader, pipelineDesc);
    m_TextureRenderData = render.createShaderProgramDataBuffer(sizeof(RenderItemData));
}

void Effect::initInstances() {
    auto& app    = Engine::Application::get();
    auto& render = app.getRender();

    m_InstanceRenderData         = render.createShaderProgramDataBuffer(sizeof(RenderItemData));
    m_InstanceMaterialRenderData = render.createShaderProgramDataBuffer(sizeof(GfxEffect::RenderMaterialData));

    RenderItemData itemData;
    itemData.model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
    itemData.model = glm::transpose(itemData.model);
    m_InstanceRenderData->copyData(&itemData);

    GfxEffect::RenderMaterialData instanceMaterial;
    instanceMaterial.diffuseAlbedo = glm::vec4(0.8f, 0.6f, 0.1f, 1.0f);
    instanceMaterial.fresnelR0     = glm::vec3(0.01f);
    instanceMaterial.roughness     = 0.5f;
    m_InstanceMaterialRenderData->copyData(&instanceMaterial);

    m_MotionRenderData = render.createShaderProgramDataBuffer(sizeof(MotionData));
}

void Effect::update(GfxEffect::RenderCommonData& commonData) {
    auto& app    = Engine::Application::get();
    auto& camera = app.getCamera();

    m_PrevView       = m_CurrView;
    m_PrevProjection = m_CurrProjection;

    m_CurrView       = commonData.view;
    m_CurrProjection = commonData.projection;
}

void Effect::draw(std::shared_ptr<Engine::CrossPlatformShaderProgramDataBuffer> commonData) {
    auto& app    = Engine::Application::get();
    auto& camera = app.getCamera();
    auto& render = app.getRender();
    auto& time   = app.getTime();

    uint32_t w, h;
    render.getViewport(w, h);

    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// PRE DRAW /////////////////////////////////
    m_Framebuffer->resize(w, h);

    render.setFramebuffer(m_Framebuffer);
    render.setPass(m_MotionDataPass);
    render.clear(0.0f, 0.0f, 0.0f, 0.0f);

    MotionData motionData;
    motionData.prevView = m_PrevView;
    motionData.prevProj = m_PrevProjection;
    m_MotionRenderData->copyData(&motionData);

    m_MotionDataShader->setDataSlot(0, commonData);
    m_MotionDataShader->setDataSlot(1, m_InstanceRenderData);
    m_MotionDataShader->setDataSlot(2, m_InstanceMaterialRenderData);
    m_MotionDataShader->setDataSlot(3, m_MotionRenderData);

    render.drawItem("motion-blur", "instance");

    RenderItemData skyboxData;
    skyboxData.model = glm::mat4(1.0f);
    m_SkyboxRenderData->copyData(&skyboxData);

    render.setPass(m_SkyboxRenderPass);
    m_SkyboxShader->setDataSlot(0, commonData);
    m_SkyboxShader->setDataSlot(1, m_SkyboxRenderData);
    m_SkyboxShader->setDataSlot(2, m_MotionRenderData);

    render.drawItem("motion-blur", "box");

    render.setFramebuffer(nullptr);
    ///////////////////////////////// PRE DRAW /////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    /////////////////////////////// COMPUTE BLUR ///////////////////////////////
    render.setComputePass(m_ComputePass);

    BlurData blurData;
    blurData.bufferSize = glm::vec2(w, h);
    m_BlurData->copyData(&blurData);

    m_BlurBuffer->resize(w, h);

    m_ComputeShader->setDataSlot(0, m_BlurData);
    m_ComputeShader->setTextureSlot(1, m_ColorBuffer);
    m_ComputeShader->setTextureSlot(2, m_MotionBuffer);
    m_ComputeShader->setReadWriteTextureSlot(3, m_BlurBuffer);

    size_t numGroupsX = static_cast<size_t>(std::ceilf(w / 16.0f));
    size_t numGroupsY = static_cast<size_t>(std::ceilf(h / 16.0f));
    render.compute(numGroupsX, numGroupsY, 1);
    /////////////////////////////// COMPUTE BLUR ///////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////// DRAW ///////////////////////////////////
    render.setPass(m_TextureRenderPass);
    RenderItemData itemData;
    itemData.model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0, 1.0));
    m_TextureRenderData->copyData(&itemData);

    m_TextureShader->setDataSlot(0, m_TextureRenderData);
    m_TextureShader->setTextureSlot(1, m_BlurBuffer);
    render.drawItem("motion-blur", "plane");
}

}  // namespace MotionBlur