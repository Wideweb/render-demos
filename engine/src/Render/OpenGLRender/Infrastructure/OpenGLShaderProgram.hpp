#pragma once

#include <memory>
#include <string>
#include <vector>

#include "OpenGLDepthStencilTexture.hpp"
#include "OpenGLRenderTexture.hpp"
#include "OpenGLShaderProgramDataBuffer.hpp"
#include "OpenGLUtils.hpp"
#include "ShaderProgramSlot.hpp"

namespace Engine {

class OpenGLShaderProgram {
public:
    OpenGLShaderProgram(
        const std::string& vertexFile, const std::string& pixelFile, const std::vector<ShaderProgramSlotDesc>& slots
    ) noexcept;

    ~OpenGLShaderProgram();

    void release();

    void setDataSlot(size_t index, std::shared_ptr<OpenGLShaderProgramDataBuffer> buffer);
    void setTextureSlot(size_t index, std::shared_ptr<OpenGLRenderTexture> renderTexture);
    void setTextureSlot(size_t index, std::shared_ptr<OpenGLDepthStencilTexture> renderTexture);

    void bind() const;
    void unbind() const;

private:
    struct Slot {
        GLint                 location;
        GLint                 binding;
        ShaderProgramSlotDesc desc;
    };

    GLuint            m_ProgramId;
    std::vector<Slot> m_Slots;
};

}  // namespace Engine