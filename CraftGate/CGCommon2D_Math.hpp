
#ifndef _CG_COMMON2D_MATH_H
#define _CG_COMMON2D_MATH_H

namespace cg
{
    f32 CGAngle(CGPointf dir)
    {
        f32 hyp = sqrt(dir.X * dir.X + dir.Y * dir.Y);
        f32 adj = dir.X;
        return acosf(adj / hyp);
    }

    s32 CGAngleToMotionDirection(CGPointf dir)
    {
        long dir_case = (long)(0.5f + CGAngle(dir) * 4.f / M_PI);
        if (dir.Y > 0.f) dir_case = ((4 - dir_case) + 4);
        dir_case = (8 - ((dir_case + 5) % 8)) % 8;
        return dir_case;
    }
}

#endif //_CG_COMMON2D_MATH_H