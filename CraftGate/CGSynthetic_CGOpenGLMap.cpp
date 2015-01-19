
#include "CGGraph_PCH.h"
#include "CGSynthetic_CGOpenGLMap.hpp"

namespace cg
{

CGPointi CGOpenGLMap::GetPosition(s32 x, s32 y) const
{
    return CGPointi((x + y) * MapGridWidth, (y - x) * MapGridHeight);
}

CGPointi CGOpenGLMap::GetIndex(s32 x, s32 y)
{
    x /= MapGridWidth;
    y /= MapGridHeight;
    return CGPointi((x - y) / 2, (x + y) / 2);
}

CGPointi CGOpenGLMap::GetIndex(CGPointi pos)
{
    return GetIndex(pos.X, pos.Y);
}

CGOpenGLGraph CGOpenGLMap::GetGraph(s32 x, s32 y, u32 layer) const
{
    if (x < 0 || x >= (s32)size.X ||
        y < 0 || y >= (s32)size.Y)
        return CGOpenGLGraph();

    u16 i;
    switch (layer)
    {
    case 0:
        i = mappedData[x + y * size.X].L0;
        break;
    case 1:
        i = mappedData[x + y * size.X].L1;
        break;
    default:
        return CGOpenGLGraph();
    }

    if (i == CGMap_InvalidData)
        return CGOpenGLGraph();

    return graphLib[i];
}

CGSizei CGOpenGLMap::Last() const
{
    return CGSizei(size.X - 1, size.Y - 1);
}

CGPointi CGOpenGLMap::GetCenter() const
{
    return GetPosition(size.X / 2, size.Y / 2);
}

}
