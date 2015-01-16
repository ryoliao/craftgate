
#ifndef _CG_OPENGL_2D_SHADER_H_
#define _CG_OPENGL_2D_SHADER_H_

namespace cg
{

static GLchar const _CGOpenGL_2D_VS[] =

    "attribute vec2 inVertexPosition;"
    "attribute vec2 inVertexTexcoord;"

    "uniform vec2 uScale;"

    "varying vec2 varVertexTexcoord;"

    "void main(void)"
    "{"
        "varVertexTexcoord = inVertexTexcoord;"
        "vec2 pos = (inVertexPosition * uScale) + vec2(-1.0, 1.0);"
        "gl_Position = vec4(pos, 0.0, 1.0);"
    "}";

static GLchar const _CGOpenGL_2D_PS[] =

    "uniform sampler2D uTexture;"

    "varying vec2 varVertexTexcoord;"

    "void main(void)"
    "{"
        "gl_FragColor = texture2D(uTexture, varVertexTexcoord);"
    "}";

static GLchar const _CGOpenGL_CLEAR_VS[] =

    "attribute vec2 inVertexPosition;"
    "attribute vec4 inVertexColor;"

    "varying vec4 varVertexColor;"

    "void main(void)"
    "{"
        "varVertexColor = inVertexColor;"
        "gl_Position = vec4(inVertexPosition, 0.0, 1.0);"
    "}";

static GLchar const _CGOpenGL_CLEAR_PS[] =

    "varying vec4 varVertexColor;"

    "void main(void)"
    "{"
        "gl_FragColor = varVertexColor;"
    "}";

}

#endif //_CG_OPENGL_2D_SHADER_H_