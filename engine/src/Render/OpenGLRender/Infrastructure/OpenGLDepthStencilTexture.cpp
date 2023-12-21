#include "OpenGLDepthStencilTexture.hpp"

#include <stdexcept>

namespace Engine {

OpenGLDepthStencilTexture::OpenGLDepthStencilTexture(GLenum format, size_t width, size_t height) {
    m_Format     = format;
    m_DataFormat = m_Format == GL_DEPTH24_STENCIL8 ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT;
    m_DataType   = m_Format == GL_DEPTH_COMPONENT32 ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_INT;
    m_Width      = width;
    m_Height     = height;

    glGenTextures(1, &m_Resource);
    glBindTexture(GL_TEXTURE_2D, m_Resource);

    glTexImage2D(GL_TEXTURE_2D, 0, m_Format, m_Width, m_Height, 0, m_DataFormat, m_DataType, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D, 0);

    setClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // // Texture Image Unit 0 will treat it as a depth texture
    // glActiveTexture (GL_TEXTURE0);
    // glBindTexture   (GL_TEXTURE_2D, depth_stencil_tex);
    // glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);

    // // Texture Image Unit 1 will treat the stencil view of depth_stencil_tex accordingly
    // glActiveTexture (GL_TEXTURE1);
    // glBindTexture   (GL_TEXTURE_2D, stencil_view);
    // glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
}

OpenGLDepthStencilTexture::~OpenGLDepthStencilTexture() { release(); }

void OpenGLDepthStencilTexture::resize(size_t width, size_t height) {
    if (width == m_Width && height == m_Height) {
        return;
    }

    if (width > UINT32_MAX || height > UINT32_MAX) {
        throw std::out_of_range("Invalid width/height");
    }

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, m_Format, width, height, 0, m_Format, m_DataType, NULL);
    unbind();

    m_Width  = width;
    m_Height = height;
}

void OpenGLDepthStencilTexture::bind() const { glBindTexture(GL_TEXTURE_2D, m_Resource); }

void OpenGLDepthStencilTexture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void OpenGLDepthStencilTexture::release() {
    if (m_Resource > 0) {
        glDeleteTextures(1, &m_Resource);
    }
}

}  // namespace Engine