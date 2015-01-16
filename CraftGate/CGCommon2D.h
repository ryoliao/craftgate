
#ifndef _CG_COMMON2D_H
#define _CG_COMMON2D_H

namespace cg
{

template<typename T>
struct CGPoint
{
    CGPoint()
    {}

    CGPoint(T x, T y)
        : X(x), Y(y)
    {}

    template<typename U>
    CGPoint(CGPoint<U> const & v)
        : X(T(v.X)), Y(T(v.Y))
    {}

    template<typename U>
    CGPoint operator+ (CGPoint<U> v) const
    {
        return CGPoint(T(X + v.X), T(Y + v.Y));
    }

    template<typename U>
    CGPoint operator- (CGPoint<U> v) const
    {
        return CGPoint(T(X - v.X), T(Y - v.Y));
    }

    template<typename U>
    CGPoint operator/ (CGPoint<U> v) const
    {
        return CGPoint(T(X / v.X), T(Y / v.Y));
    }

    T X, Y;
};

typedef CGPoint<s32> CGPointi;
typedef CGPoint<u32> CGSizei;
typedef CGPoint<f32> CGPointf;

template<typename T>
struct CGRect
{
    CGRect()
    {}

    CGRect(T left, T right, T top, T bottom)
        : LeftTop(left, top), RightBottom(right, bottom)
    {}

    template<typename U>
    CGRect(CGPoint<T> pt, CGPoint<U> size)
        : LeftTop(pt), RightBottom(pt + size)
    {}

    template<typename U>
    CGRect(CGRect<U> const & other)
        : LeftTop(other.LeftTop)
        , RightBottom(other.RightBottom)
    {}

    void merge(CGRect const & v)
    {
        LeftTop.X = std::min(LeftTop.X, v.LeftTop.X);
        LeftTop.Y = std::min(LeftTop.Y, v.LeftTop.Y);
        RightBottom.X = std::max(RightBottom.X, v.RightBottom.X);
        RightBottom.Y = std::max(RightBottom.Y, v.RightBottom.Y);
    }

    T getWidth() const
    {
        return RightBottom.X - LeftTop.X;
    }

    T getHeight() const
    {
        return RightBottom.Y - LeftTop.Y;
    }

    CGPoint<T> getSize() const
    {
        return CGPoint<T>(getWidth(), getHeight());
    }

    CGPoint<T> getRightTop() const
    {
        return CGPoint<T>(RightBottom.X, LeftTop.Y);
    }

    CGPoint<T> getLeftBottom() const
    {
        return CGPoint<T>(LeftTop.X, RightBottom.Y);
    }

    CGPoint<T> toClient(CGPoint<T> v)
    {
        return v - LeftTop;
    }

    CGPoint<T> flipClientPos(CGRect<T> v, bool flipX, bool flipY)
    {
        CGPoint<T> flip = v.LeftTop;
        if (flipX)
            flip.X = getWidth() - v.getWidth() - v.LeftTop.X;
        if (flipY)
            flip.Y = getHeight() - v.getHeight() - v.LeftTop.Y;
        return flip;
    }

    CGPoint<T> LeftTop;
    CGPoint<T> RightBottom;
};

typedef CGRect<s32> CGRecti;
typedef CGRect<f32> CGRectf;

}

#endif //_CG_RECT_H