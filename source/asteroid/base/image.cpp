#include "asteroid/base/image.h"

using namespace Asteroid;

Image::Image(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height)
{
    Allocate();
}

Image::~Image()
{
    Release();
}

void Image::SetData(const void* data)
{
    cudaGraphicsMapResources(1, &m_resource, 0);

    cudaArray* texture_ptr;
    cudaGraphicsSubResourceGetMappedArray(&texture_ptr, m_resource, 0, 0);

    size_t size = m_Width * m_Height * sizeof(uchar4);
    cudaMemcpyToArray(texture_ptr, 0, 0, data, size, cudaMemcpyDeviceToDevice);
    
    cudaGraphicsUnmapResources(1, &m_resource, 0);
}

void Image::Resize(unsigned int width, unsigned int height)
{
    if (m_Width == width && m_Height == height)
    {
		return;
    }

    Release();

    m_Width = width;
    m_Height = height;

    Allocate();
}

void Image::Allocate()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, GL_RGBA8, m_Width, m_Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	cudaGraphicsGLRegisterImage(&m_resource, m_RendererID, GL_TEXTURE_2D, cudaGraphicsRegisterFlagsWriteDiscard);
}

void Image::Release()
{
    cudaGraphicsUnregisterResource(m_resource);

    glDeleteTextures(1, &m_RendererID);
}
