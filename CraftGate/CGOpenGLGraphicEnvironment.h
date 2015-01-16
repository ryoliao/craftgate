
#ifndef _CG_OPENGL_GRAPHIC_ENVIRONMENT_H_
#define _CG_OPENGL_GRAPHIC_ENVIRONMENT_H_

#if _CG_USE_OPENGL

namespace cg
{

enum E_CG_DIRECTION
{
    CG_LEFT     = 0x1,
    CG_RIGHT    = 0x2,
    CG_TOP      = 0x4,
    CG_BOTTOM   = 0x8,
};

class CGOpenGLTexture;

class CGOpenGLGraphicEnvironment
{
public:
    CGOpenGLGraphicEnvironment();
    ~CGOpenGLGraphicEnvironment();

    void setViewport(CGRecti viewport);
    void drawImage(CGOpenGLTexture const & tex, CGRecti rect, u32 direction=CG_LEFT|CG_TOP);
    void drawImage(CGOpenGLTexture const & tex, CGRecti sourceRect, CGRecti targetRect, u32 direction=CG_LEFT|CG_TOP);
    void drawImage(CGOpenGLTexture const & tex, CGPointi pos, u32 direction=CG_LEFT|CG_TOP);
    void FillColor(CGColor clr);
    void GradientFillLinear(CGColor initialClr, CGColor dstClr, u32 direction);

private:
    enum
    {
        ST_UNKNOWN,
        ST_CLEAR,
        ST_2D,
    };

    void initBasisProgram();
    void beginStateClear();
    void beginState2D();
    void doDrawRect(CGRecti rect, CGPointf const * texcoords=0);
    void doBindTexture(CGOpenGLTexture const * tex);

    // state 2D
    GLuint program2D;
    GLint u2D_Scale;
    GLint u2D_Texture;

    // state clear
    GLuint programClear;

    // current state
    s32 stat;

    CGRecti mViewport;
};

}

#endif //_CG_USE_OPENGL
#endif //_CG_OPENGL_GRAPHIC_ENVIRONMENT_H_