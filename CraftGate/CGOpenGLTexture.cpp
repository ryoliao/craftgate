
#include "CGGraph_PCH.h"

namespace cg
{

CGOpenGLTexture::CGOpenGLTexture()
    : td(0) 
    , szTexture(0, 0)
{
}

CGOpenGLTexture::CGOpenGLTexture(CGGraphRef graph)
    : td(0)
    , szTexture(0, 0)
{
    reset(graph);
}

CGOpenGLTexture::~CGOpenGLTexture()
{
    if (td)
        glDeleteTextures(1, &td);
}

void CGOpenGLTexture::reset(CGGraphRef graph)
{
    if (graph &&
        graph->palette)
    {
        if (0 == td)
            glGenTextures(1, &td);

        defGraphInfo const & desc = graph->desc;
        std::vector<u8> const & indexBuffer = graph->buffer;
        CGPalette const & pal = graph->palette.get();
        u32 szGraph = desc.width * desc.height;
        std::vector<u32> imgBuffer;

        imgBuffer.resize(szGraph);
        for (u32 i=0; i<szGraph; ++i)
            imgBuffer[i] = pal.getGL_RGBA(indexBuffer[i]);

        glBindTexture(GL_TEXTURE_2D, td);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc.width, desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuffer.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        szTexture.X = desc.width;
        szTexture.Y = desc.height;
    }
    else
    if (td)
    {
        glDeleteTextures(1, &td);
        td=0;
        szTexture = CGSizei(0, 0);
    }
}

GLuint CGOpenGLTexture::getName() const
{
    return td;
}

CGSizei CGOpenGLTexture::getSize() const
{
    return szTexture;
}

bool CGOpenGLTexture::isOK() const
{
    return (0 != td);
}

}
