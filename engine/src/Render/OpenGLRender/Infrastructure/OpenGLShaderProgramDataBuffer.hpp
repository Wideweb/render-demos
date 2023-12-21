#pragma once

#include "OpenGLUtils.hpp"

namespace Engine {

class OpenGLShaderProgramDataBuffer {
public:
    OpenGLShaderProgramDataBuffer(size_t byteSize);

    OpenGLShaderProgramDataBuffer(const OpenGLShaderProgramDataBuffer& rhs)            = delete;
    OpenGLShaderProgramDataBuffer& operator=(const OpenGLShaderProgramDataBuffer& rhs) = delete;

    ~OpenGLShaderProgramDataBuffer();

    void copyData(void* data);
    void bind() const;
    void unbind() const;
    void release();

    GLuint getResource() const noexcept { return m_Resource; }

private:
    GLuint m_Resource;
    size_t m_ByteSize;
};

class OpenGLShaderProgramTextureSlot {
public:
    GLuint resource;
    GLuint location;
};

}  // namespace Engine