
#ifndef _CG_SYNTHETIC_OPENGL_MAP_H_
#define _CG_SYNTHETIC_OPENGL_MAP_H_

namespace cg
{
    template<typename Fn>
    void CGOpenGLMap::EnumerateObjects(CGRecti rect, Fn & f)
    {
        // add border
        rect.LeftTop.Y -= MapGridHeight;
        rect.RightBottom.X += MapGridWidth;
        rect.RightBottom.Y += MapGridHeight;

        CGPointi lt = GetIndex(rect.LeftTop);
        CGPointi rt = GetIndex(rect.getRightTop());
        CGPointi lb = GetIndex(rect.getLeftBottom());

        s32 bx = lt.X;
        s32 nx = rt.X - bx;
        s32 by = lt.Y;
        s32 ny = lb.Y - by;

        auto drawer = [=, &f](s32 lx, s32 ly, u32 layer)
        {
            for (s32 x = 0; x <= nx; ++x)
            {
                s32 fx = lx + x;
                s32 fy = ly + x;

                CGOpenGLGraph graph = GetGraph(fx, fy, layer);
                if (graph.tex.isOK())
                    f(graph.tex.get(), GetPosition(fx, fy) + graph.offset);
            }
        };

        auto bounder = [=, &drawer](u32 layer)
        {
            for (s32 y = 0; y <= ny; ++y)
            {
                s32 lx = bx - y;
                s32 ly = by + y;
                drawer(lx, ly, layer);
                drawer(lx, ly + 1, layer);
            }
        };

        bounder(0);
        bounder(1);
    }
}

#endif //_CG_SYNTHETIC_OPENGL_MAP_H_