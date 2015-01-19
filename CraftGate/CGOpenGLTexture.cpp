
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

bool CGOpenGLTexture::reset(CGGraphRef graph, s32 x, s32 y, u32 width, u32 height)
{
    if (graph &&
        graph->palette)
    {
        defGraphInfo const & desc = graph->desc;

        // prevent out of graph frame
        if (x >= desc.width || y >= desc.height)
            goto fail;
        if (x + width >= (u32)desc.width)
            width = desc.width - x;
        if (y + height >= (u32)desc.height)
            height = desc.height - y;

        if (0 == td)
            glGenTextures(1, &td);

        std::vector<u8> const & indexBuffer = graph->buffer;
        CGPalette const & pal = graph->palette.get();
        u32 szGraph = width * height;
        std::vector<u32> imgBuffer;

        imgBuffer.resize(szGraph);
        for (u32 iy = 0; iy < height; ++iy)
        {
            u32 offYSource = (iy + y) * desc.width;
            u32 offYTarget = iy * width;
            for (u32 ix = 0; ix < width; ++ix)
            {
                u32 offSource = offYSource + ix + x;
                u32 offTarget = offYTarget + ix;
                imgBuffer[offTarget] = pal.getGL_RGBA(indexBuffer[offSource]);
            }
        }

        glBindTexture(GL_TEXTURE_2D, td);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuffer.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        szTexture.X = width;
        szTexture.Y = height;
        return true;
    }

    fail:
    if (td)
    {
        glDeleteTextures(1, &td);
        td = 0;
        szTexture = CGSizei(0, 0);
    }
    return false;
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
