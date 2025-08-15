#pragma once 

class Framebuffer
{
public:
    Framebuffer(unsigned int width, unsigned int height);
    ~Framebuffer();

    void bind();
    void unbind();

    // Resizes the framebuffer and its attachments
    void resize(unsigned int width, unsigned int height);

    unsigned int getTextureID() const { return m_ColorAttachment; }

private:
    // Helper function to create/recreate the framebuffer
    void invalidate();

    unsigned int m_RendererID = 0;
    unsigned int m_ColorAttachment = 0;
    unsigned int m_DepthAttachment = 0;
    unsigned int m_Width, m_Height;
};