#pragma once

#include "OpenGLComputeProgram.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace Engine {

OpenGLComputeProgram::OpenGLComputeProgram(
    const std::string& file, const std::vector<ShaderProgramSlotDesc>& slots
) noexcept {
    GLuint shader = OpenGLUtils::compileShader(file, GL_COMPUTE_SHADER);
    if (shader == 0) {
        return;
    }

    m_ProgramId = glCreateProgram();
    if (m_ProgramId == 0) {
        throw std::runtime_error("Failed to create gl program");
    }

    glAttachShader(m_ProgramId, shader);
    glLinkProgram(m_ProgramId);

    GLint linked_status = 0;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &linked_status);
    if (linked_status == 0) {
        GLint infoLen = 0;
        glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &infoLen);

        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(m_ProgramId, infoLen, nullptr, infoLog.data());

        throw std::runtime_error(infoLog.data());
        glDeleteProgram(m_ProgramId);
        return;
    }

    glDetachShader(m_ProgramId, shader);
    glDeleteShader(shader);

    GLint dataSlots    = 0;
    GLint textureSlots = 0;

    m_Slots.resize(slots.size());
    for (int i = 0; i < m_Slots.size(); i++) {
        auto type = slots[i].type;

        if (type == SHADER_PROGRAM_SLOT_TYPE::DATA || type == SHADER_PROGRAM_SLOT_TYPE::DATA_ARRAY) {
            m_Slots[i].location = glGetUniformBlockIndex(m_ProgramId, slots[i].name.c_str());
            m_Slots[i].binding  = dataSlots++;
        } else if (type == SHADER_PROGRAM_SLOT_TYPE::TEXTURE) {
            m_Slots[i].location = glGetUniformLocation(m_ProgramId, slots[i].name.c_str());
            m_Slots[i].binding  = textureSlots++;
        }
    }
}

OpenGLComputeProgram::~OpenGLComputeProgram() { release(); }

void OpenGLComputeProgram::setDataSlot(size_t index, std::shared_ptr<OpenGLShaderProgramDataBuffer> buffer) {
    glUniformBlockBinding(m_ProgramId, m_Slots[index].location, m_Slots[index].binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_Slots[index].binding, buffer->getResource());
}

void OpenGLComputeProgram::setReadWriteDataSlot(size_t index, std::shared_ptr<DxReadWriteDataBuffer> buffer) {
    m_CommandList->SetComputeRootUnorderedAccessView(index, buffer->resource()->GetGPUVirtualAddress());
}

void OpenGLComputeProgram::setReadWriteTextureSlot(size_t index, std::shared_ptr<OpenGLRenderTexture> renderTexture) {
    glBindImageTexture(m_Slots[index].binding, renderTexture->getResource(), 0, GL_FALSE, 0, GL_READ_WRITE, renderTexture->getFormat());
}

void OpenGLComputeProgram::setTextureSlot(size_t index, std::shared_ptr<OpenGLRenderTexture> renderTexture) {
    glActiveTexture(GL_TEXTURE0 + m_Slots[index].binding);
    glBindTexture(GL_TEXTURE_2D, renderTexture->getResource());
    glUniform1i(m_Slots[index].location, m_Slots[index].binding);
}

void OpenGLComputeProgram::setTextureSlot(size_t index, std::shared_ptr<OpenGLDepthStencilTexture> dsTexture) {
    glActiveTexture(GL_TEXTURE0 + m_Slots[index].binding);
    glBindTexture(GL_TEXTURE_2D, dsTexture->getResource());
    glUniform1i(m_Slots[index].location, m_Slots[index].binding);
}

void OpenGLComputeProgram::bind() const { glUseProgram(m_ProgramId); }

void OpenGLComputeProgram::unbind() const { glUseProgram(0); }

void OpenGLComputeProgram::release() {
    if (m_ProgramId > 0) {
        glDeleteProgram(m_ProgramId);
        m_ProgramId = 0;
    }
}

}  // namespace Engine