
#ifndef _CG_OPENGL_TEXTURE_H_
#define _CG_OPENGL_TEXTURE_H_

#if _CG_USE_OPENGL

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
    GLuint getName() const;
    CGSizei getSize() const;

private:
    GLuint td;
    CGSizei szTexture;
};

typedef CGSharedRef<CGOpenGLTexture> CGOpenGLTextureRef;

}

#endif
#endif