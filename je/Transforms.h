#pragma once

#include "Types.h"

namespace je::vec
{
    inline Vec2f Centre(Vec2f vec, Vec2f origin)
    {
        return Vec2f{
                vec.x - origin.x,
                vec.y - origin.y};
    }

    inline Vec2f Scale(Vec2f vec, Vec2f scale)
    {
        return Vec2f{
                vec.x * scale.x,
                vec.y * scale.y};
    }

    inline Vec2f Rotate(Vec2f vec, Vec2f rotate)
    {
        return Vec2f{
                vec.x * rotate.x - vec.y * rotate.y,
                vec.x * rotate.y + vec.y * rotate.x};
    }

    inline Vec2f Translate(Vec2f vec, Vec2f translate)
    {
        return Vec2f{
                vec.x + translate.x,
                vec.y + translate.y};
    }

    inline Vec2f Transform(Vec2f vec, Vec2f centre, Vec2f scale, Vec2f rotate, Vec2f translate)
    {
        vec = Centre(vec, centre);
        vec = Scale(vec, scale);
        vec = Rotate(vec, rotate);
        vec = Translate(vec, translate);
        return vec;
    }
}// namespace je::vec
