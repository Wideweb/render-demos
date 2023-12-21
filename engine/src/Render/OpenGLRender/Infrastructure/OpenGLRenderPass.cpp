#include "OpenGLRenderPass.hpp"

#include "OpenGLUtils.hpp"

namespace Engine {

OpenGLRenderPass::OpenGLRenderPass(std::shared_ptr<OpenGLShaderProgram> shaderProgram, PipelineDesc pipelineDesc) {
    m_ShaderProgram = shaderProgram;
    m_PipelineDesc  = pipelineDesc;
}

void OpenGLRenderPass::bind() const {
    glFrontFace(GL_CCW);

    if (m_PipelineDesc.cullMode == GL_NONE) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(m_PipelineDesc.cullMode); // GL_FRONT | GL_BACK
    }

    if (m_PipelineDesc.depthFunc == GL_NONE) {
        glDisable(GL_DEPTH_TEST);
    } else {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(m_PipelineDesc.depthFunc); // GL_LESS | GL_GREATER
    }

    m_ShaderProgram->bind();
}

void OpenGLRenderPass::unbind() const { m_ShaderProgram->unbind(); }

}  // namespace Engine