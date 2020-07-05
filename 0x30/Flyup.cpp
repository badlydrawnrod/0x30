#include "Flyup.h"

#include "je/MyTime.h"
#include "je/QuadHelpers.h"
#include "je/Textures.h"

Flyup::Flyup(je::TextureRegion& texture, float x, float y, float duration)
    : texture_{texture}, x_{x}, y_{y}, endTime_{je::GetTime() + duration}
{
}

Flyup::Flyup(const Flyup& other)
    : texture_{other.texture_}, x_{other.x_}, y_{other.y_}, endTime_{other.endTime_}
{
}

Flyup& Flyup::operator=(const Flyup& other)
{
    if (&other != this)
    {
        texture_ = other.texture_;
        x_ = other.x_;
        y_ = other.y_;
        endTime_ = other.endTime_;
    }

    return *this;
}

void Flyup::Draw(je::Batch& batch)
{
    batch.AddVertices(je::quads::Create(texture_, x_, y_));
    y_ -= 0.25f;
}

bool Flyup::IsAlive() const
{
    return je::GetTime() < endTime_;
}
