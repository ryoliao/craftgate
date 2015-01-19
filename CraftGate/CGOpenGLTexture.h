
#ifndef _CG_OPENGL_TEXTURE_H_
#define _CG_OPENGL_TEXTURE_H_

namespace cg
{

class CGOpenGLTexture
{
public:
    CGOpenGLTexture();
    CGOpenGLTexture(CGGraphRef graph);
    ~CGOpenGLTexture();

    bool isOK() const;

    void reset(CGGraphRef graph);
    bool reset(CGGraphRef graph, u32 x, u32 y, u32 width, u32 height);
    GLuint getName() const;
    CGSizei getSize() const;

private:
    GLuint td;
    CGSizei szTexture;
};

typedef CGSharedRef<CGOpenGLTexture> CGOpenGLTextureRef;

}

#endif