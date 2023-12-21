#include "OpenGLShaderProgramDataBuffer.hpp"

namespace Engine {

OpenGLShaderProgramDataBuffer::OpenGLShaderProgramDataBuffer(size_t byteSize) {
    glGenBuffers(1, &m_Resource);
    glBindBuffer(GL_UNIFORM_BUFFER, m_Resource);
    glBufferData(GL_UNIFORM_BUFFER, byteSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // glBindBufferBase(GL_UNIFORM_BUFFER, i, m_Resource);

    m_ByteSize = byteSize;
}

OpenGLShaderProgramDataBuffer::~OpenGLShaderProgramDataBuffer() {}

void OpenGLShaderProgramDataBuffer::copyData(void* data) {
    bind();
    glBufferData(GL_UNIFORM_BUFFER, m_ByteSize, data, GL_STATIC_DRAW);
    unbind();
}

void OpenGLShaderProgramDataBuffer::bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_Resource); }

void OpenGLShaderProgramDataBuffer::unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

void OpenGLShaderProgramDataBuffer::release() {
    if (m_Resource > 0) {
        glDeleteBuffers(1, &m_Resource);
        m_Resource = 0;
    }
}

}  // namespace Engine