#pragma once

#include <memory>

#include "DxUtils.hpp"
#include "OpenGLShaderProgram.hpp"
#include "glad/glad.h"

namespace Engine {

class OpenGLRenderPass {
public:
    struct PipelineDesc {
        GLenum cullMode        = GL_BACK;
        GLenum depthFunc       = GL_LESS;
    };

    OpenGLRenderPass(std::shared_ptr<OpenGLShaderProgram> shaderProgram, PipelineDesc pipelineDesc);

    void bind() const;
    void unbind() const;

private:
    std::shared_ptr<OpenGLShaderProgram> m_ShaderProgram;
    PipelineDesc                         m_PipelineDesc;
};

}  // namespace Engine