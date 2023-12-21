#pragma once
#include <memory>
#include <string>
#include <vector>

#include "OpenGLDepthStencilTexture.hpp"
#include "OpenGLRenderTexture.hpp"
#include "OpenGLShaderProgramDataBuffer.hpp"
#include "OpenGLUtils.hpp"
#include "ShaderProgramSlot.hpp"
#include "glad/glad.h"

namespace Engine {

class OpenGLComputeProgram {
public:
    OpenGLComputeProgram(const std::string& file, const std::vector<ShaderProgramSlotDesc>& slots) noexcept;

    ~OpenGLComputeProgram();

    void release();

    void setDataSlot(size_t index, std::shared_ptr<OpenGLShaderProgramDataBuffer> buffer);
    void setReadWriteTextureSlot(size_t index, std::shared_ptr<OpenGLRenderTexture> renderTexture);
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