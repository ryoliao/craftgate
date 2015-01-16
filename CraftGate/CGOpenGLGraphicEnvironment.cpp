
#include "CGGraph_PCH.h"

#if _CG_USE_OPENGL

#include "CGOpenGL2D.shader.h"

namespace cg
{

struct SGLColor 
{
    SGLColor()
    {}

    SGLColor(CGColor const & clr)
        : R(clr.r), G(clr.g), B(clr.b), A(0xff)
    {}

    u8 R, G, B, A;
};

static GLboolean glAttachShaderFromString(GLuint program, GLchar const * code, GLenum type)
{
    GLuint shader;
    GLint result;

    /* create shader object, set the source, and compile */
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    /* make sure the compilation was successful */
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLchar * log;
        GLint length;

        /* get the shader info log */
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        log = (GLchar *)alloca(length);
        glGetShaderInfoLog(shader, length, &result, log);

        /* print an error message and the info log */
        CGError::error("glCreateShaderFromString(): Unable to compile shader :\n%s\n", log);

        glDeleteShader(shader);
        return GL_FALSE;
    }

    glAttachShader(program, shader);
    glDeleteShader(shader);
    return GL_TRUE;
}

struct SAttribLocation
{
    c8 const * attribName;
    u32 index;
};

static GLuint glCreateProgramVF(GLchar const * vs, GLchar const * ps, SAttribLocation* defAttribs=0)
{
    GLuint program;
    GLint result;

    program = glCreateProgram();

    /* create program object and attach shaders */
    if (!glAttachShaderFromString(program, vs, GL_VERTEX_SHADER)
        || !glAttachShaderFromString(program, ps, GL_FRAGMENT_SHADER))
    {
        glDeleteProgram(program);
        program = 0;
    }

    /* link the program and make sure that there were no errors */
    if (program)
    {
        for (; defAttribs->attribName; ++defAttribs)
            glBindAttribLocation(program, defAttribs->index, defAttribs->attribName);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if(result == GL_FALSE)
        {
            GLint length;
            GLchar * log;

            /* get the program info log */
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            log = (GLchar *)alloca(length);
            glGetProgramInfoLog(program, length, &result, log);

            /* print an error message and the info log */
            CGError::error("glCreateProgramVPS(): Program linking failed:\n%s\n", log);

            /* delete the program */
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

///////////////////////////////////////////////////////////////////////////////

CGOpenGLGraphicEnvironment::CGOpenGLGraphicEnvironment()
    : stat(ST_UNKNOWN)
{
    initBasisProgram();
}

CGOpenGLGraphicEnvironment::~CGOpenGLGraphicEnvironment()
{
    if (program2D)
        glDeleteProgram(program2D);
}

void CGOpenGLGraphicEnvironment::initBasisProgram()
{
    static SAttribLocation attribs2D[] =
    {
        "inVertexPosition", 0,
        "inVertexTexcoord", 1,
        0
    };

    program2D = glCreateProgramVF(_CGOpenGL_2D_VS, _CGOpenGL_2D_PS, attribs2D);
    if (program2D)
    {
        u2D_Scale = glGetUniformLocation(program2D, "uScale");
        u2D_Texture = glGetUniformLocation(program2D, "uTexture");
    }

    static SAttribLocation attribsClear[] =
    {
        "inVertexPosition", 0,
        "inVertexColor", 1,
        0
    };

    programClear = glCreateProgramVF(_CGOpenGL_CLEAR_VS, _CGOpenGL_CLEAR_PS, attribsClear);

    glReleaseShaderCompiler();
}

void CGOpenGLGraphicEnvironment::setViewport(CGRecti viewport)
{
    mViewport = viewport;

    // setup viewport
    glViewport(mViewport.LeftTop.X, mViewport.LeftTop.Y, mViewport.getWidth(), mViewport.getHeight());

    // update uniform
    CGPointi szViewport = viewport.getSize();
    glUniform2f(u2D_Scale, 2.0f / szViewport.X, -2.0f / szViewport.Y);
}

void CGOpenGLGraphicEnvironment::FillColor(CGColor clr)
{
    glClearColor(clr.r / 255.0f, clr.g / 255.0f, clr.b / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void CGOpenGLGraphicEnvironment::GradientFillLinear(CGColor initialClr, CGColor dstClr, u32 direction)
{
    beginStateClear();

    CGPointf positions[4] =
    {
        CGPointf(-1.0f, 1.0f),
        CGPointf( 1.0f, 1.0f),
        CGPointf(-1.0f, -1.0f),
        CGPointf(1.0f, -1.0f)
    };

    SGLColor colors[4] =
    {
        (direction & (CG_LEFT|CG_TOP)) ? initialClr : dstClr,
        (direction & (CG_RIGHT|CG_TOP)) ? initialClr : dstClr,
        (direction & (CG_LEFT|CG_BOTTOM)) ? initialClr : dstClr,
        (direction & (CG_RIGHT|CG_BOTTOM)) ? initialClr : dstClr
    };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CGPointf), positions);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SGLColor), colors);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CGOpenGLGraphicEnvironment::drawImage(CGOpenGLTexture const & tex, CGRecti rect, u32 direction/*=CG_LEFT|CG_TOP*/)
{
    beginState2D();
    doBindTexture(&tex);

    f32 x0 = (direction & CG_LEFT)  ? 0.0f : 1.0f;
    f32 x1 = (direction & CG_RIGHT) ? 0.0f : 1.0f;
    f32 y0 = (direction & CG_TOP)   ? 0.0f : 1.0f;
    f32 y1 = (direction & CG_BOTTOM)? 0.0f : 1.0f;

    CGPointf texcoords[4] =
    {
        CGPointf(x0, y0),
        CGPointf(x1, y0),
        CGPointf(x0, y1),
        CGPointf(x1, y1)
    };

    doDrawRect(rect, texcoords);
}

void CGOpenGLGraphicEnvironment::drawImage(CGOpenGLTexture const & tex, CGRecti sourceRect, CGRecti targetRect, u32 direction/*=CG_LEFT|CG_TOP*/)
{
    beginState2D();
    doBindTexture(&tex);

    CGRectf r(sourceRect);
    r.LeftTop = r.LeftTop / tex.getSize();
    r.RightBottom = r.RightBottom / tex.getSize();

    f32 x0 = (direction & CG_LEFT)  ? r.LeftTop.X : r.RightBottom.X;
    f32 x1 = (direction & CG_RIGHT) ? r.LeftTop.X : r.RightBottom.X;
    f32 y0 = (direction & CG_TOP)   ? r.LeftTop.Y : r.RightBottom.Y;
    f32 y1 = (direction & CG_BOTTOM)? r.LeftTop.Y : r.RightBottom.Y;

    CGPointf texcoords[4] =
    {
        CGPointf(x0, y0),
        CGPointf(x1, y0),
        CGPointf(x0, y1),
        CGPointf(x1, y1)
    };

    doDrawRect(targetRect, texcoords);
}

void CGOpenGLGraphicEnvironment::drawImage(CGOpenGLTexture const & tex, CGPointi pos, u32 direction/*=CG_LEFT|CG_TOP*/)
{
    CGRecti rect(pos, tex.getSize());
    drawImage(tex, rect, direction);
}

void CGOpenGLGraphicEnvironment::doDrawRect(CGRecti rect, CGPointf const * texcoords/*=0*/)
{
    if (!texcoords)
    {
        glDisableVertexAttribArray(1);
        glVertexAttrib2f(1, 0.0f, 0.0f);
    }
    else
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CGPointf), texcoords);
    }

    CGPointf points[4] =
    {
        rect.LeftTop,
        rect.getRightTop(),
        rect.getLeftBottom(),
        rect.RightBottom
    };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CGPointf), points);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CGOpenGLGraphicEnvironment::beginState2D()
{
    if (stat == ST_2D)
        return;

    // setup program
    glUseProgram(program2D);
    glUniform1i(u2D_Texture, 0);

    // setup states
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glStencilMask(0);
    glActiveTexture(0);

    stat = ST_2D;
}

void CGOpenGLGraphicEnvironment::beginStateClear()
{
    if (stat == ST_CLEAR)
        return;

    // setup program
    glUseProgram(programClear);

    // setup states
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glStencilMask(0);
    glActiveTexture(0);

    stat = ST_CLEAR;
}

void CGOpenGLGraphicEnvironment::doBindTexture(CGOpenGLTexture const * tex)
{
    glBindTexture(GL_TEXTURE_2D, tex ? tex->getName() : 0);
}

}

#endif